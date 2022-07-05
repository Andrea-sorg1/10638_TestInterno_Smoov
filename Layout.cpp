//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>
#include <stdio.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <share.h>
#include <errno.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <functional>

#pragma hdrstop

#include "Main.h"
#include "Remoto.h"
#include "Layout.h"
#include "Stato.h"
#include "Macwin.h"
#include "Magwin.h"
#include "Baiwin.h"
#include "BatWin.h"
#include "Nodowin.h"
#include "OmronETH.h"
#include "Tecnofer.h"
#include "XYNodi.h"
#include "Agvwin.h"
#include "Stato.h"
#include "GrpDest.h"
#include "Impianti.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma resource "*.dfm"
TFormLayout *FormLayout;
//----------------------------------------------------
// Variabili definite in GRAPHVAR.CPP
//----------------------------------------------------
extern int   offset[][2];
extern int   offset1[][2];
extern int   OffSV1;
extern int   OffSO1;
extern float RidAll;
extern float RidMin;
extern float RidMax;
extern float Rap;
// ---------------------------
//   PRECEDENTE DATI AGV
// ---------------------------
extern int PrecAvanti[MAXAGV+1];              // precedente direzione avanti
extern int PrecIndietro[MAXAGV+1];            // precedente direzione indietro
extern int PrecOffDaNod[MAXAGV+1];            // precedente offset da nodo
extern int PrecGrdSuNod[MAXAGV+1];            // precedente direzione avanti
//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];
//  ---------------------
//   STRUTTURA PERCORSO
//  ---------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo
//-------------------------------------------------------
extern int Percorso[][2];
// --------------------------
//  STRUTTURA PercorsoCurva
// --------------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo        _         _
//  2 = Quadrante:           0 =   |   1 = |    2 = _|   3 = |_
//  3 = Raggio   :    espresso in cm come i nodi ( SE 0 se la calcola la funzione
//                    presupponendo che la curva parta tangente a X o Y )
//  4 = PAGINA 1-2-3... (Numero pagina grafica di cui la curva è da visualizzare)
//--------------------------------------------------------------------
extern int PercorsoCurva[][5];
// ---------------------------------
//  STRUTTURA PercorsoCurva BEZIER
//  0 = grado curva
//      0 - disattivata
//      1 - curva lineare (no curva = linea)
//      2 - quadratica ( 3 vertici di controllo)
//      3 - cubica     ( 4 vertici di controllo)
//  1 = Numero nodo ingresso curva (primo nodo controllo)
//  2 = Numero nodo uscita curva   (ultimo nodo controllo)
//  3 = Coordinata X del 2° nodo controllo
//  4 = Coordinata Y del 2° nodo controllo
//  5 = Coordinata X del 3° nodo controllo  (solo per CUBICHE)
//  6 = Coordinata Y del 3° nodo controllo  (solo per CUBICHE)
//--------------------------------------------------------------------
extern int PercorsoCurvaBezier[][7];

// --------------------------
//   STRUTTURE GRAFICHE
// --------------------------
//
extern int Macch[][5];
extern int Magaz[][5];
extern int Baie[][4];
extern int CBattGraph[][5];
extern int AccoppiaMagaz[][2];

//  ---------------------
//     STRUTTURA PORTE
//  ---------------------
//  1  - Ascissa  (X) per la sigla della porta
//  2  - Ordinata (Y) per la sigla della porta
//  3  - Ascissa  (X1) per la visualizzazione della porta
//  4  - Ordinata (Y1) per la visualizzazione della porta
//  5  - Ascissa  (X2) per la visualizzazione della porta
//  6  - Ordinata (Y2) per la visualizzazione della porta
//---------------------------------------------------------
extern int Porte[][6];

//---------------------------------------------------------------------------
__fastcall TFormLayout::TFormLayout(TComponent* Owner)
  : TForm(Owner)
{
  int i=0;

  for(i=1;i<=MAXAGV;i++){
     IngombroAgvGrafico[i] = new ingombroagv(i);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormCreate(TObject *Sender)
{
  int Grassetto = SIGLE_GRASSETTO;

  clSFONDO    = clBlack;
  clLINEE     = clLtGray;
  clNODI      = clGray;
  clNODI_FING = clBlue;
  clNUMLOC    = clWhite;
  //----------------------------------------------------------
  // Inizializzazione dell'oggetto grafico
  //----------------------------------------------------------
  Sfondo  = new Graphics::TBitmap();
  Sfondo1 = new Graphics::TBitmap();
  //----------------------------------------------------------
  // Attiva/Disattiva scritte in grassetto
  //----------------------------------------------------------
  if(Grassetto==1){
     Sfondo->Canvas->Font->Style  = Sfondo->Canvas->Font->Style<<fsBold;
     Sfondo1->Canvas->Font->Style = Sfondo1->Canvas->Font->Style<<fsBold;
  }
  //----------------------------------------------------------
  // Inizializzazione del colore dello sfondo
  //----------------------------------------------------------
  FormLayout->Color  = clSFONDO;
  //----------------------------------------------------------
  // StaticText con dati carrello
  //----------------------------------------------------------
  TextDati->Color    = clSFONDO;
  TextDati->Caption  = "";
  TextDati->Visible  = false;
  //----------------------------------------------------------
  // Visualizzazione dati associati ai carrelli
  //----------------------------------------------------------
//Application->ShowHint  = true;
//Application->HintColor = clAqua;
//Application->HintPause = 1000;
  Application->OnHint    = &OnHint;
  //----------------------------------------------------------
  // *** Inizializza tutte le variabili grafiche globali ***
  //----------------------------------------------------------
  InizializzaVariabiliGrafiche();
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  switch (Key) {
     //===========================================
     // "PAGE UP"
     // Zoom -
     //===========================================
     case VK_NEXT:     PosizionamentoSuNodo=0;
                       //-----------------------------------------------
                       // Zoom - in sinottico pricipale
                       //-----------------------------------------------
                       Rid=Rid-0.03;
                       if(Rid<RidMin) Rid=RidMin;
                       break;
     //===========================================
     // "PAGE DOWN"
     // Zoom +
     //===========================================
     case VK_PRIOR:    PosizionamentoSuNodo=0;
                       //-----------------------------------------------
                       // Zoom + in sinottico pricipale
                       //-----------------------------------------------
                       Rid=Rid+0.03;
                       if(Rid>RidMax) Rid=RidMax;
                       break;
  }
  //--------------------------------------------------------------------
  // Nel Caso si Abilitata la magina di modifica coordinate passa l'ESC
  // Alla FormXYNodi per deselezionare i nodi scelti
  //--------------------------------------------------------------------
  if(Key==VK_ESCAPE && ModificaCoordinateNodi==true){
     FormXYNodi->FormKeyDown(Sender,Key,Shift);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormResize(TObject *Sender)
{
  //----------------------------------------------
  // Recupero le dimensioni della Form principale
  //----------------------------------------------
  FormW = FormLayout->Width;
  FormH = FormLayout->Height;
  //----------------------------------------------
  // Calcolo le dimensioni dello Sfondo
  //----------------------------------------------
  SfondoW = (int)(FormW);
  SfondoH = (int)(FormH);
  if(SfondoW<1) SfondoW=1;
  if(SfondoH<1) SfondoH=1;
  //----------------------------------------------
  // Azzero lo sfondo rifacendolo nero
  //----------------------------------------------
  Sfondo->Width  = SfondoW;
  Sfondo->Height = SfondoH;
  Sfondo->Canvas->Pen->Width   = 1;
  Sfondo->Canvas->Pen->Color   = clSFONDO;
  Sfondo->Canvas->Pen->Style   = psSolid;
  Sfondo->Canvas->Brush->Color = clSFONDO;
  Sfondo->Canvas->Brush->Style = bsSolid;
  Sfondo->Canvas->Rectangle(0, 0, SfondoW, SfondoH);
  //----------------------------------------------
  // Ridisegnamento impianto
  //----------------------------------------------
  Canvas->Draw(0, 0, Sfondo);    // Layout
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormMouseWheelUp(TObject *Sender,  TShiftState Shift, TPoint &MousePos, bool &Handled)
{
  //-----------------------------------------------
  // Azzeramento posizionamento grafico a nodo
  //-----------------------------------------------
  PosizionamentoSuNodo=0;
  //-----------------------------------------------
  // Incremento il rapporto di riduzione
  //-----------------------------------------------
  Rid=Rid+0.03;
  if(Rid>RidMax) Rid=RidMax;
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled)
{
  //-----------------------------------------------
  // Azzeramento posizionamento grafico a nodo
  //-----------------------------------------------
  PosizionamentoSuNodo=0;
  //-----------------------------------------------
  // Incremento il rapporto di riduzione
  //-----------------------------------------------
  Rid=Rid-0.03;
  if(Rid<RidMin) Rid=RidMin;
}
//---------------------------------------------------------------------------
  ///------------------------------------------
  // Cattura l'evento OnHint dei componenti
  // che hanno la proprietà Hint attiva e
  // lo visualizza sulla Barra di Stato
  //------------------------------------------
void __fastcall TFormLayout::OnHint(TObject *Sender)
{
  char buffer[1001]="";
  bool visibile=false;
  TColor colore;

  TextDati->Visible=visibile;
  ShowHint=visibile;
  FormMain->SBarImpianto->Panels->Items[0]->Text ="";

  strcpy(buffer, Application->Hint.c_str());
  // --- Macchine ---
  if(buffer[0]=='M'){
     TextDati->Caption=Application->Hint;
     TextDati->Height = 91;
     TextDati->Width  = 216;
     colore=clWhite;
     visibile=true;
  }
  // --- Magazzini ---
  if(buffer[0]=='m'){
     TextDati->Caption=Application->Hint;
     TextDati->Height = 91;
     TextDati->Width  = 216;
     if(GestioneUdc>0){
        TextDati->Height = 150;
        TextDati->Width  = 300;
     }
     colore=clFuchsia;
     visibile=true;
  }
  // --- Baie ---
  if(buffer[0]=='b'){
     TextDati->Caption=Application->Hint;
     TextDati->Height = 91;
     TextDati->Width  = 216;
     colore=clAqua;
     visibile=true;
  }
  //-----------------------------------------------
  if(ModernHint==false){
     TextDati->Visible=visibile;
     TextDati->Font->Color=colore;
  }
  else{
     ShowHint=visibile;
     Application->HintColor = (TColor) 0x00F3E2D9;
     Application->HintPause=100;
     Application->HintShortPause=1;
     Application->HintHidePause=10000; // 10 Sec.
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       FormMouseDown()
// ----------------------------------------------
void __fastcall TFormLayout::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  int  pos;
  bool elenco_pos;
  char buffer[120]="";
  char NomeFile[100]="";
  short int svuota_mag, qty, grpmag, prevstato;
  short int i, j, z, s;
  int NewCurvaSel, NumCBatt;
  int x_mouse, y_mouse, grd;
  int xcen, ycen, xpal, ypal;
  int x, y, mezzo;
  int yic, yfc, xic, xfc, x_max, y_max, x_min, y_min, offs1;
  int xinc1, xinc2, xinc3, xinc4;
  int yinc1, yinc2, yinc3, yinc4;
  int num_nod, nodo_p, sviluppo, posti;
  int larg_cest = LARG_CEST;
  int lung_cest = LUNG_CEST;
  int carico_in_punta  = CARICO_IN_PUNTA;
  int dist_tra_palette = DIST_CEST_IN_MAG;
  int x_attivo, y_attivo, offx_attivo, offy_attivo;
  float Rid_attivo;
  short int NumMagAccoppiato;
  short int tipo_box, offset_da_nodo;
  long double offsX_nod, offsY_nod, offsX, offsY, hip;
  long double sinA, cosA, sinB, cosB, sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double radA_m, radA, radB, radC, radB1, radB2;
  long double ComponenteXOffset, ComponenteYOffset;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  int  err, result;
  int agv_per_ingombro;

  //--------------------------------------------------
  // Variabili per gestione modificacurve di bezier
  //--------------------------------------------------
  int xpuntoa;
  int ypuntoa;
  int xpuntob;
  int ypuntob;
  int xpuntoc;
  int ypuntoc;
/*
  int d1;
  int d2;
  int d;
  double a;
  double b;
  double m;
  int q;
  double new_dist, old_dist=99999999999999999;
*/
  double xt,yt;
  double ax,ay;
  double bx,by;
  double t;
  double step;
  TPoint points[5];


  x_mouse=X;
  y_mouse=Y;
  FormNodi->NodoIniziale = 0;
  //-----------------------------------------------------
  // Memorizzo parametri di puntamento del Mouse
  //-----------------------------------------------------
  offx_attivo = offset[1][0];
  offy_attivo = offset[1][1];
  Rid_attivo  = Rid;
  x_attivo    = x_mouse;
  y_attivo    = y_mouse;
  //-----------------------------------------------------
  // Blocco i controlli del mouse fino a che la pagina è
  // in apertura
  //-----------------------------------------------------
  if(FormMain->FormInOpen==true) return;
  //-----------------------------------------------------
  // *** NODI DEL PERCORSO ***
  //-----------------------------------------------------
  mezzo=(int)(((40/Rap)*Rid_attivo)/2);
  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  //-----------------------------------------------------
  for(i=1;i<=MAXPUNTI;i++){
     if(Punti[ i ][2]==0) break;
     if(Punti[ i ][4]==0) continue;
     if(Punti[ i ][0]==0 &&
        Punti[ i ][1]==0) continue;
     //------------------------------------------------------
     // calcola coordinate stazione
     //------------------------------------------------------
     x= ((Punti[ i ][0] -offx_attivo)/Rap)*Rid_attivo;
     y= ((Punti[ i ][1] -offy_attivo)/Rap)*Rid_attivo;
     //------------------------------------------------------
     // Disegno un quadrato intorno alla ricarica
     //------------------------------------------------------
     xic = x-mezzo;
     xfc = x+mezzo;
     yic = y-mezzo;
     yfc = y+mezzo;
     //------------------------------------------------------
     // Se il mouse viene premuto all'interno dell'area
     // occupata da una macchina allora visualizza la Pagina
     // di editor della macchina
     //------------------------------------------------------
     if(yic > yfc){
        y_max=  yic;
        y_min=  yfc;
     }
     else{
        y_max=  yfc;
        y_min=  yic;
     }
     if(xic > xfc){
        x_max=  xic;
        x_min=  xfc;
     }
     else{
        x_max=  xfc;
        x_min=  xic;
     }
     if(( y_attivo > y_min )&&( y_attivo < y_max )){
        if(( x_attivo > x_min )&&( x_attivo < x_max )){
           //--------------------------------------
           // Ricarica dei TGV
           //--------------------------------------
           FormNodi->NodoIniziale =(short int)(i);
           break;
        }
     }
  }

  //RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
  // apre l'editor TGV se si clicca nel rettangolo calcolato in disegnaAgv (Pos_Agv)
  FormTgv->NumeroTgv=0;
  if(TestPercorsiTecnoFerrari==false){
     mezzo = (long double)(((LUNG_AGV/Rap)*Rid_attivo)/2);
     for(i=1; i<=MAXAGV; i++){
        xic = Pos_Agv[i].xc-mezzo;
        xfc = Pos_Agv[i].xc+mezzo;
        yic = Pos_Agv[i].yc-mezzo;
        yfc = Pos_Agv[i].yc+mezzo;
        //------------------------------------------------------
        // Se il mouse viene premuto all'interno dell'area
        // occupata da una macchina allora visualizza la Pagina
        // di editor della macchina
        //------------------------------------------------------
        if(yic > yfc){
           y_max=  yic;
           y_min=  yfc;
        }
        else{
           y_max=  yfc;
           y_min=  yic;
        }
        if(xic > xfc){
           x_max=  xic;
           x_min=  xfc;
        }
        else{
           x_max=  xfc;
           x_min=  xic;
        }
        if(( y_attivo > y_min )&&( y_attivo < y_max )){
           if(( x_attivo > x_min )&&( x_attivo < x_max )){
              FormTgv->NumeroTgv = i;
           }
        }
     }
  }
  //RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // di una *** MACCHINA ***
  //-----------------------------------------------------
  FormMacchine->NumeroMacchina=0;
  FormMacchine->NumeroStazione=0;
  FormMacchine->NumeroPosizione=0;
  for(i=1; i<=TOTCHIAMATE; i++ ){                // per tutte le macchine
     //-------------------------------------------------------
     // Abilitata la visualizzazione zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(P.ch[i]->Num<1           ) continue;
     if(P.ch[i]->Num>TOTCHIAMATE ) continue;

     //-------------------------------------------------------
     // Recupero dati legati alla singola macchina
     //-------------------------------------------------------
     sviluppo = Macch[i][0];                    // Direzione di sviluppo (direzione da
                                                // primo cestone depositato -> a nodo)
                                                // primo cestone depositato -> a nodo)
     //-------------------------------------------------------
     // Ciclo per tutte le stazioni
     //-------------------------------------------------------
     for(s=0; s<MAXSTAZMAC; s++){                // per tutte le stazioni
        //-------------------------------------------------------
        // dati
        //-------------------------------------------------------
        num_nod  = P.ch[i]->Staz[s].PuntoIng;     // Numero del nodo di imgresso magazzino
        posti    = P.ch[i]->Staz[s].Npt;          // Posti previsti su stazione
        if(num_nod==0) continue;                  // Se il Nodo del Magazzino=0 ---> Skip!
        //-------------------------------------------------------
        // Locazione inesistente  (posti=0)
        //-------------------------------------------------------
        if(posti==0  ) continue;

        //-------------------------------------------------------
        // 30/04/97 Disegna con l'offset relativo alla pagina
        //-------------------------------------------------------
        xcen = ((Punti[num_nod][0]-offx_attivo));            // pixel x del nodo occupato
        ycen = ((Punti[num_nod][1]-offy_attivo));            // pixel y del nodo occupato

        //-------------------------------------------------------
        // Recupero i gradi di rotazione in base allo sviluppo
        // assegnato
        //-------------------------------------------------------
        grd = sviluppo;
        //-------------------------------------------------------
        // Aggiustamento dello sviluppo standard ai gradi
        //-------------------------------------------------------
        if(sviluppo==0) grd=0;
        if(sviluppo==1) grd=180;
        if(sviluppo==2) grd=90;
        if(sviluppo==3) grd=270;

        //-------------------------------------------------------
        // Gradi to Radianti
        //-------------------------------------------------------
        radA_m = DegToRad(grd);
        //-------------------------------------------------------
        // Calcolo delle dimensioni grafiche della paletta
        //-------------------------------------------------------
        MezzoLatoX = (long double)(larg_cest/2);
        MezzoLatoY = (long double)(lung_cest/2);
        //-------------------------------------------------------
        // Inizilizzo offSet di posizionamento PALETTA
        //-------------------------------------------------------
        offsX_nod = 0;
        offsY_nod = 0;  // inizializzazione con offset manuale
        if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
        else{
           offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
        }
        //-------------------------------------------------------
        // Disegno di tutte le postazioni
        //-------------------------------------------------------
        for(j=1; j<=posti; j++){
           //-------------------------------------------------------
           // Incremento offset posizione TGV da nodo
           //-------------------------------------------------------
           offsX = offsX_nod;
           offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette));
           //--------------------------------------
           // Recupero gradi singola stazione
           //--------------------------------------
           radA = radA_m;
           if(P.ch[i]->Staz[s].RotLayout!=0){
              radA = DegToRad(P.ch[i]->Staz[s].RotLayout);
           }
           //--------------------------------------
           // Visualizzazione tanti riquadri
           // quante le tot posizini in presa
           //--------------------------------------
           for(z=0; z<Macch[i][2]; z++){
              //-------------------------------------------------------
              // Calcolo coordinate grafiche del centro paletta
              //-------------------------------------------------------
              offsX = (((larg_cest * (Macch[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
              //-------------------------------------------------------
              // ipotenusa del triangolo rettangolo i cui lati sono
              // gli offset del centro paletta dal centro navetta
              //-------------------------------------------------------
              radB  = (long double)(atanl( offsX / offsY ));
              SinCos(radB, sinB, cosB);
              hip   = offsY / cosB;
              //-------------------------------------------------------
              // Calcolo nell'angolo di rotazione del centro paletta
              // come sommatoria dei gradi del TGV + l'angolo di
              // rotazione dal centro TGV al centro paletta
              //-------------------------------------------------------
              radC = radA + radB;
              SinCos(radC, sinC, cosC);
              //-------------------------------------------------------
              // Offset in X,Y del centro PALLET rispetto al centro TGV
              //-------------------------------------------------------
              ComponenteYOffset = hip * cosC;
              ComponenteXOffset = hip * sinC;

              xpal = xcen-(int)(ComponenteXOffset);
              ypal = ycen-(int)(ComponenteYOffset);
              //-------------------------------------------------------
              // Recupero coordinate vertici box per visualizzazione
              //-------------------------------------------------------
              radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

              radB1 = radA + radB;
              radB2 = radA - radB;
              SinCos(radB1, sinB1, cosB1);
              SinCos(radB2, sinB2, cosB2);

              MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

              ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
              ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
              ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
              ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

              xinc1 = (int) (xpal-(int)(ComponenteX1));
              xinc2 = (int) (xpal+(int)(ComponenteX2));
              xinc3 = (int) (xpal+(int)(ComponenteX1));
              xinc4 = (int) (xpal-(int)(ComponenteX2));

              yinc1 = (int) (ypal+(int)(ComponenteY1));
              yinc2 = (int) (ypal-(int)(ComponenteY2));
              yinc3 = (int) (ypal-(int)(ComponenteY1));
              yinc4 = (int) (ypal+(int)(ComponenteY2));

              //-----------------------------------------------
              // Aggiustamenti dovuti ad arrotondamenti
              //-----------------------------------------------
              if(grd==90 || grd==270){
                 offs1 = abs(xinc4-xinc3);
                 if(offs1<3) xinc4=xinc3;
                 offs1 = abs(xinc2-xinc1);
                 if(offs1<3) xinc2=xinc1;
                 offs1 = abs(yinc4-yinc1);
                 if(offs1<3) yinc4=yinc1;
                 offs1 = abs(yinc3-yinc2);
                 if(offs1<3) yinc3=yinc2;
              }
              if(grd==0 || grd==180){
                 offs1 = abs(xinc4-xinc1);
                 if(offs1<3) xinc4=xinc1;
                 offs1 = abs(xinc3-xinc2);
                 if(offs1<3) xinc3=xinc2;
                 offs1 = abs(yinc2-yinc1);
                 if(offs1<3) yinc2=yinc1;
                 offs1 = abs(yinc4-yinc3);
                 if(offs1<3) yinc4=yinc3;
              }

              //------------------------------------------------------
              // Se il mouse viene premuto all'interno dell'area
              // occupata da una macchina allora visualizza la Pagina
              // di editor della macchina
              //------------------------------------------------------
              xic = xinc1/Rap*Rid_attivo;
              xfc = xinc3/Rap*Rid_attivo;
              yic = yinc1/Rap*Rid_attivo;
              yfc = yinc3/Rap*Rid_attivo;
              if(yic > yfc){
                 y_max=  yic;
                 y_min=  yfc;
              }
              else{
                 y_max=  yfc;
                 y_min=  yic;
              }
              if(xic > xfc){
                 x_max=  xic;
                 x_min=  xfc;
              }
              else{
                 x_max=  xfc;
                 x_min=  xic;
              }
              if(( y_attivo > y_min )&&( y_attivo < y_max )){
                 if(( x_attivo > x_min )&&( x_attivo < x_max )){
                    FormMacchine->NumeroMacchina=i;
                    FormMacchine->NumeroStazione=j-1;
                    FormMacchine->NumeroPosizione=z;
                    break;
                 }
              }
           }
           if(FormMacchine->NumeroMacchina!=0) break;
        }
        if(FormMacchine->NumeroMacchina!=0) break;
     }
  }

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // di una *** BAIE ***
  //-----------------------------------------------------
  FormBaie->NumeroBaia = 0;
  for(i=0; i<=TOTBAIE; i++){                // per tutte le baie
     //-------------------------------------------------------
     // Abilitata la visualizzazione zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(P.ba[i]->Num<1       ) continue;
     if(P.ba[i]->Num>TOTBAIE ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     lung_cest  = LUNG_CEST;
     num_nod    = P.ba[i]->Punto;      // Numero del nodo di imgresso magazzino
     sviluppo   = Baie[i][0];          // Direzione di sviluppo (direzione da
                                       // primo cestone depositato -> a nodo)
     posti      = Baie[i][1];          // Massimo numero cestoni
     if(num_nod==0          ) continue;     // Se il Nodo del Magazzino=0 ---> Skip!
     if(Punti[num_nod][4]==0) continue;     // pagina non impostata
     //-------------------------------------------------------
     // Locazione inesistente  (posti=0)
     //-------------------------------------------------------
     if(posti==0) continue;

     //-------------------------------------------------------
     // 30/04/97 Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offx_attivo));            // pixel x del nodo occupato
     ycen = ((Punti[num_nod][1]-offy_attivo));            // pixel y del nodo occupato

     //-------------------------------------------------------
     // Recupero i gradi di rotazione in base allo sviluppo
     // assegnato
     //-------------------------------------------------------
     grd = sviluppo;
     //-------------------------------------------------------
     // Aggiustamento dello sviluppo standard ai gradi
     //-------------------------------------------------------
     if(sviluppo==0) grd=0;
     if(sviluppo==1) grd=180;
     if(sviluppo==2) grd=90;
     if(sviluppo==3) grd=270;
     //-------------------------------------------------------
     // Gradi to Radianti
     //-------------------------------------------------------
     radA = DegToRad(grd);
     //-------------------------------------------------------
     // Calcolo delle dimensioni grafiche della paletta
     //-------------------------------------------------------
     MezzoLatoX = (long double)(larg_cest/2);
     MezzoLatoY = (long double)(lung_cest/2);

     //-------------------------------------------------------
     // Inizilizzo offSet di posizionamento PALETTA
     //-------------------------------------------------------
     offsX_nod = 0;
     offsY_nod = Baie[i][3];  // inizializzazione con offset manuale
     if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
     else{
        offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
     }
     //-------------------------------------------------------
     // Disegno di tutte le postazioni
     //-------------------------------------------------------
     for(j=1; j<=posti; j++){
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsX = offsX_nod;
        offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette));
        //--------------------------------------
        // Visualizzazione tanti riquadri
        // quante le tot posizini in presa
        //--------------------------------------
        for(z=0; z<Baie[i][2]; z++){
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (Baie[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);

           //-------------------------------------------------------
           // Recupero coordinate vertici box per visualizzazione
           //-------------------------------------------------------
           radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           radB1 = radA + radB;
           radB2 = radA - radB;
           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

           xinc1 = (int) (xpal-(int)(ComponenteX1));
           xinc2 = (int) (xpal+(int)(ComponenteX2));
           xinc3 = (int) (xpal+(int)(ComponenteX1));
           xinc4 = (int) (xpal-(int)(ComponenteX2));

           yinc1 = (int) (ypal+(int)(ComponenteY1));
           yinc2 = (int) (ypal-(int)(ComponenteY2));
           yinc3 = (int) (ypal-(int)(ComponenteY1));
           yinc4 = (int) (ypal+(int)(ComponenteY2));

           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xinc4-xinc3);
              if(offs1<3) xinc4=xinc3;
              offs1 = abs(xinc2-xinc1);
              if(offs1<3) xinc2=xinc1;
              offs1 = abs(yinc4-yinc1);
              if(offs1<3) yinc4=yinc1;
              offs1 = abs(yinc3-yinc2);
              if(offs1<3) yinc3=yinc2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xinc4-xinc1);
              if(offs1<3) xinc4=xinc1;
              offs1 = abs(xinc3-xinc2);
              if(offs1<3) xinc3=xinc2;
              offs1 = abs(yinc2-yinc1);
              if(offs1<3) yinc2=yinc1;
              offs1 = abs(yinc4-yinc3);
              if(offs1<3) yinc4=yinc3;
           }

           //------------------------------------------------------
           // Se il mouse viene premuto all'interno dell'area
           // occupata da una macchina allora visualizza la Pagina
           // di editor della macchina
           //------------------------------------------------------
           xic = xinc1/Rap*Rid_attivo;
           xfc = xinc3/Rap*Rid_attivo;
           yic = yinc1/Rap*Rid_attivo;
           yfc = yinc3/Rap*Rid_attivo;
           if(yic > yfc){
              y_max=  yic;
              y_min=  yfc;
           }
           else{
              y_max=  yfc;
              y_min=  yic;
           }
           if(xic > xfc){
              x_max=  xic;
              x_min=  xfc;
           }
           else{
              x_max=  xfc;
              x_min=  xic;
           }
           if(( y_attivo > y_min )&&( y_attivo < y_max )){
              if(( x_attivo > x_min )&&( x_attivo < x_max )){
                 FormBaie->NumeroBaia = i;     // Apertura Editor su primo record
                 break;
              }
           }
        }
        if(FormBaie->NumeroBaia!=0) break;
     }
     if(FormBaie->NumeroBaia!=0) break;
  }

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // **** CAMBIOBATTERIE ****
  //-----------------------------------------------------
  FormCambioBatteria->NumCBatt = 0;

  for(i=0; i<=TOTBATT; i++){
     //-------------------------------------------------------
     // Modifica coorinate nodi
     //-------------------------------------------------------
     if(ModificaCoordinateNodi==true) break;
     //-------------------------------------------------------
     // Abilitata la visualizzazione zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Test percorsi Tecnoferrari --> skip!
     //-------------------------------------------------------
     if(TestPercorsiTecnoFerrari==true) break;
     //-------------------------------------------------------
     // condizioni di skip
     //-------------------------------------------------------
     if(P.cb[i]->Num<1       ) continue;
     if(P.cb[i]->Num>TOTBATT ) continue;
     if(P.cb[i]->Punto==0    ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     num_nod = P.cb[i]->Punto;  // Numero del nodo
     //-------------------------------------------------------
     // Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offset[1][0])); // pixel x del nodo
     ycen = ((Punti[num_nod][1]-offset[1][1])); // pixel y del nodo
     //-------------------------------------------------------
     // Inizializzo offset di posizionamento
     //-------------------------------------------------------
     offsX = CBattGraph[i][0];  // offset manuale X
     offsY = CBattGraph[i][1];  // offset manuale Y
     //-----------------------------------------------
     // Punti dei vertici del poligono
     //-----------------------------------------------
     xinc1 = xcen - MezzoLatoX + offsX;    yinc1 = ycen - MezzoLatoY + offsY;
     xinc2 = xcen + MezzoLatoX + offsX;    yinc2 = ycen - MezzoLatoY + offsY;
     xinc3 = xcen + MezzoLatoX + offsX;    yinc3 = ycen + MezzoLatoY + offsY;
     xinc4 = xcen - MezzoLatoX + offsX;    yinc4 = ycen + MezzoLatoY + offsY;

     //------------------------------------------------------
     // Se il mouse viene premuto all'interno dell'area
     // occupata da una macchina allora visualizza la Pagina
     // di editor della macchina
     //------------------------------------------------------
     xic = xinc1/Rap*Rid_attivo;   yic = yinc1/Rap*Rid_attivo;
     xfc = xinc3/Rap*Rid_attivo;   yfc = yinc3/Rap*Rid_attivo;

     if(yic > yfc){ y_max = yic;   y_min = yfc; }
     else{          y_max = yfc;   y_min = yic; }

     if(xic > xfc){ x_max = xic;   x_min = xfc; }
     else{          x_max=  xfc;   x_min=  xic; }

     if(( y_attivo > y_min )&&( y_attivo < y_max )){
        if(( x_attivo > x_min )&&( x_attivo < x_max )){
           FormCambioBatteria->NumCBatt = i;     // Apertura Editor su primo record
           break;
        }
     }
     // if(FormCambioBatteria->NumCBatt!=0) break;
  }

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // di una *** MAGAZZINI ***
  //-----------------------------------------------------
  FormMagaz->NumeroMagazzino = 0;
  FormMagaz->NumeroPosizione = 0;
  for(i=1; i<=TOTMAG; i++){                // per tutte le macchine
     //-------------------------------------------------------
     // Abilitata la visualizzazione zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(M.m[i]->Num<1      ) continue;
     if(M.m[i]->Num>TOTMAG ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     num_nod   = M.m[i]->PuntoIng;          // Numero del nodo di imgresso magazzino
     sviluppo  = Magaz[i][0];               // Direzione di sviluppo (direzione da
                                            // primo cestone depositato -> a nodo)
     posti     = M.m[i]->Npt;               // Capacità magazzino
     tipo_box  = M.m[i]->TipoBox;           // Tipo di Box in magazzino

     if(num_nod==0          ) continue;     // Se il Nodo del Magazzino=0 ---> Skip!
     if(Punti[num_nod][4]==0) continue;     // pagina non impostata
     //-------------------------------------------------------
     // Locazione inesistente  (posti=0)
     //-------------------------------------------------------
     if(posti==0            ) continue;
     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     lung_cest         = M.TipiBox[tipo_box].Lung;
     larg_cest         = M.TipiBox[tipo_box].Larg;
     dist_tra_palette  = M.TipiBox[tipo_box].dist_box_in_magaz;
     offset_da_nodo    = M.m[i]->OffSetDaNodo + M.TipiBox[tipo_box].offset_tgv_da_nodo_mag;

     //-------------------------------------------------------
     // 30/04/97 Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offx_attivo));            // pixel x del nodo occupato
     ycen = ((Punti[num_nod][1]-offy_attivo));            // pixel y del nodo occupato
     //-------------------------------------------------------
     // Recupero i gradi di rotazione in base allo sviluppo
     // assegnato
     //-------------------------------------------------------
     grd = sviluppo;
     //-------------------------------------------------------
     // Aggiustamento dello sviluppo standard ai gradi
     //-------------------------------------------------------
     if(sviluppo==0) grd=0;
     if(sviluppo==1) grd=180;
     if(sviluppo==2) grd=90;
     if(sviluppo==3) grd=270;
     //-------------------------------------------------------
     // Gradi to Radianti
     //-------------------------------------------------------
     radA = DegToRad(grd);
     //-------------------------------------------------------
     // Calcolo delle dimensioni grafiche della paletta
     //-------------------------------------------------------
     MezzoLatoX = (long double)(larg_cest/2);
     MezzoLatoY = (long double)(lung_cest/2);
     //-------------------------------------------------------
     // Inizilizzo offSet di posizionamento PALETTA
     //-------------------------------------------------------
     offsX_nod = 0;
     offsY_nod = Magaz[i][1];  // inizializzazione con offset manuale
     if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
     else{
        offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
     }
     //-------------------------------------------------------
     // Disegno di tutte le postazioni
     //-------------------------------------------------------
     for(j=1; j<=posti; j++){
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsX = offsX_nod;
        offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette)) + offset_da_nodo;
        //--------------------------------------
        // Recupero eventuale nodo posizione
        //--------------------------------------
        nodo_p = M.m[i]->Pos[j].NodoIng;
        if(nodo_p>0 && nodo_p<=TOTPUNTI){
           xcen  = ((Punti[nodo_p][0]-offset[1][0]));            // pixel x del nodo occupato
           ycen  = ((Punti[nodo_p][1]-offset[1][1]));            // pixel y del nodo occupato
           offsY = offsY_nod;
        }
        //--------------------------------------
        // Scroll di tutte le posizioni in presa
        //--------------------------------------
        for(z=0; z<Magaz[i][2]; z++){
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (Magaz[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);

           //-------------------------------------------------------
           // Recupero coordinate vertici box per visualizzazione
           //-------------------------------------------------------
           radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           radB1 = radA + radB;
           radB2 = radA - radB;
           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );


           xinc1 = (int) (xpal-(int)(ComponenteX1));
           xinc2 = (int) (xpal+(int)(ComponenteX2));
           xinc3 = (int) (xpal+(int)(ComponenteX1));
           xinc4 = (int) (xpal-(int)(ComponenteX2));

           yinc1 = (int) (ypal+(int)(ComponenteY1));
           yinc2 = (int) (ypal-(int)(ComponenteY2));
           yinc3 = (int) (ypal-(int)(ComponenteY1));
           yinc4 = (int) (ypal+(int)(ComponenteY2));

           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xinc4-xinc3);
              if(offs1<3) xinc4=xinc3;
              offs1 = abs(xinc2-xinc1);
              if(offs1<3) xinc2=xinc1;
              offs1 = abs(yinc4-yinc1);
              if(offs1<3) yinc4=yinc1;
              offs1 = abs(yinc3-yinc2);
              if(offs1<3) yinc3=yinc2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xinc4-xinc1);
              if(offs1<3) xinc4=xinc1;
              offs1 = abs(xinc3-xinc2);
              if(offs1<3) xinc3=xinc2;
              offs1 = abs(yinc2-yinc1);
              if(offs1<3) yinc2=yinc1;
              offs1 = abs(yinc4-yinc3);
              if(offs1<3) yinc4=yinc3;
           }

           //------------------------------------------------------
           // Se il mouse viene premuto all'interno dell'area
           // occupata da una macchina allora visualizza la Pagina
           // di editor della macchina
           //------------------------------------------------------
           xic = xinc1/Rap*Rid_attivo;
           xfc = xinc3/Rap*Rid_attivo;
           yic = yinc1/Rap*Rid_attivo;
           yfc = yinc3/Rap*Rid_attivo;
           if(yic > yfc){
              y_max=  yic;
              y_min=  yfc;
           }
           else{
              y_max=  yfc;
              y_min=  yic;
           }
           if(xic > xfc){
              x_max=  xic;
              x_min=  xfc;
           }
           else{
              x_max=  xfc;
              x_min=  xic;
           }
           if(( y_attivo > y_min )&&( y_attivo < y_max )){
              if(( x_attivo > x_min )&&( x_attivo < x_max )){
                 FormMagaz->NumeroMagazzino = i;     // Apertura Editor su primo record
                 FormMagaz->NumeroPosizione = j;     // Apertura Editor su primo record
                 break;
              }
           }
           if(FormMagaz->NumeroMagazzino!=0) break;
        }
        if(FormMagaz->NumeroMagazzino!=0) break;
     }
  }

  //-------------------------------------------
  // TECNOFERRARI
  // Se sono in fase di Test Percorsi
  //-------------------------------------------
  if(TestPercorsiTecnoFerrari==true && FormNodi->NodoIniziale!=0){
     if(Button==mbLeft ){
        StartTestTecno = (short int)(FormNodi->NodoIniziale);
        AGV[1]->stato.pos = StartTestTecno;
        // --- rotazioni ---
        AGV[1]->stato.s.bit.grd0=0;
        AGV[1]->stato.s.bit.grd90=0;
        AGV[1]->stato.s.bit.grd180=0;
        AGV[1]->stato.s.bit.grd270=0;
        if(N.n[StartTestTecno]->rec[0].rot==0){AGV[1]->stato.s.bit.grd0=1;   AGV[1]->stato.rot_calamite=0;}
        if(N.n[StartTestTecno]->rec[0].rot==1){AGV[1]->stato.s.bit.grd90=1;  AGV[1]->stato.rot_calamite=90;}
        if(N.n[StartTestTecno]->rec[0].rot==2){AGV[1]->stato.s.bit.grd180=1; AGV[1]->stato.rot_calamite=180;}
        if(N.n[StartTestTecno]->rec[0].rot==3){AGV[1]->stato.s.bit.grd270=1; AGV[1]->stato.rot_calamite=270;}
     }
     if(Button==mbRight){
        EndTestTecno  = (short int)(FormNodi->NodoIniziale);
        TimeTestTecno = 0;  // Azzeramenti conteggio visualizzazione
     }
     return;
  }
  //------------------------------------------------
  // Selezione di un CambioBatteria con il tasto Sx
  //------------------------------------------------
  if(Button==mbLeft && FormCambioBatteria->NumCBatt!=0 && VisualizzaZoneImpianto==false && ModificaCoordinateNodi==false){
     FormCambioBatteria->btnRefresh->Enabled=true;
     NumCBatt=FormCambioBatteria->NumCBatt;
     do{
        ModalResult=mrOk;
        FormCambioBatteria->NumCBatt=NumCBatt;
        ModalResult=FormCambioBatteria->ShowModal();
     }while(ModalResult==mrRetry);
     return;
  }
  //------------------------------------------------
  // Selezione di un DECESTONATORE con il tasto SX
  //------------------------------------------------
  if(Button==mbLeft && FormMacchine->NumeroMacchina!=0 && VisualizzaZoneImpianto==false && ModificaCoordinateNodi==false){
     //------------------------------------------------
     // Apertura della Form del Mac. selezionato.
     //------------------------------------------------
     FormMacchine->ShowModal();
     return;
  }
  //------------------------------------------------
  // Selezione di una BAIA con il tasto SX
  //------------------------------------------------
  if(Button==mbLeft && FormBaie->NumeroBaia!=0 && VisualizzaZoneImpianto==false && ModificaCoordinateNodi==false){
     //------------------------------------------------
     // Apertura della Form del Mac. selezionato.
     //------------------------------------------------
     FormBaie->ShowModal();
     return;
  }
  //------------------------------------------------
  // Selezione di un MAGAZZINO con il tasto SX
  //------------------------------------------------
  if(Button==mbLeft && FormMagaz->NumeroMagazzino!=0 && VisualizzaZoneImpianto==false && ModificaCoordinateNodi==false){
     //------------------------------------------------
     // Apertura della Form del Mac. selezionato.
     //------------------------------------------------
     FormMagaz->ShowModal();
     return;
  }
  //------------------------------------------------
  // Selezione di un TGV con il tasto SX
  //------------------------------------------------
  if(Button==mbLeft && FormTgv->NumeroTgv!=0 && VisualizzaZoneImpianto==false){
     FormStato->PageControlTGV->ActivePage=FormStato->TabTGV[FormTgv->NumeroTgv];
     FormStato->PageControlTGVChange(Sender);
     FormStato->Refresh();
     FormTgv->ShowModal();
     return;
  }
  //----------------------------------------------
  // Selezione di un Nodo con il tasto SX
  //----------------------------------------------
  if(Button==mbLeft && FormNodi->NodoIniziale!=0 && ModificaCoordinateNodi==false){
     //------------------------------------------------
     // Apertura della Form della Mag. selezionato
     //------------------------------------------------
     if(LivelloPassword>2){
        FormNodi->ShowModal();
        //--------------------------------------------------
        // Richiesta di scaricamento mappa nodi (TGV LASER)
        //--------------------------------------------------
        for(i=1; i<=MAXAGV; i++){
           if(!RiattivaMapping[i]) continue;
           sprintf(buffer, "Attivare scaricamento mappa coordinate per TGV%d?", i);
           result = Application->MessageBox(buffer, MESS[99], MB_YESNO | MB_ICONQUESTION);
           if(result != IDYES) continue;
           //--------------------------------------------
           // Apertura della finestra di gestione mappa
           //--------------------------------------------
           FormMappa->NumTgv=i;
           FormMappa->ShowModal();
        }
     }
     return;
  }
  //----------------------------------------------
  // GESTIONE X/Y NODI
  //----------------------------------------------
  if(Button==mbLeft && ModificaCoordinateNodi==true){
     agv_per_ingombro=atoi(FormXYNodi->meAGVsel->Text.c_str());
     if(agv_per_ingombro>=1 && agv_per_ingombro<=MAXAGV){
        FormXYNodi->SegmentoPerIngombro[agv_per_ingombro]= 999999;
        FormXYNodi->CurvaPerIngombro[agv_per_ingombro]=    999999;
        FormXYNodi->PuntoPerIngombro[agv_per_ingombro]=    999999;
     }

     //---------------------------------------------------------
     // Seleziona segmento
     //---------------------------------------------------------
     for(i=0;;i++){
        if(Percorso[i][0]==0) break;

        if(Punti[ Percorso[i][0] ][4]==0 || Punti[ Percorso[i][1] ][4]==0) continue;

        xpuntoa = ((Punti[ Percorso[i][0] ][0] - offx_attivo)/Rap)*Rid_attivo;
        ypuntoa = ((Punti[ Percorso[i][0] ][1] - offy_attivo)/Rap)*Rid_attivo;
        xpuntob = ((Punti[ Percorso[i][1] ][0] - offx_attivo)/Rap)*Rid_attivo;
        ypuntob = ((Punti[ Percorso[i][1] ][1] - offy_attivo)/Rap)*Rid_attivo;

        if(xpuntob!=xpuntoa){
           t = (double)(x_attivo - xpuntoa) / (double)(xpuntob - xpuntoa);
           if(t<0) continue;
           if(t>1) continue;
        }
        else{
           if( x_attivo != xpuntob) continue;
           if( y_attivo<min((double)ypuntoa, (double)ypuntob) ) continue;
           if( y_attivo>max((double)ypuntoa, (double)ypuntob) ) continue;
        }

        if(ypuntob!=ypuntoa){
           t = (double)(y_attivo - ypuntoa) / (double)(ypuntob - ypuntoa);
           if(t<0) continue;
           if(t>1) continue;
        }
        else{
           //continue;
           if( y_attivo != ypuntob) continue;
           if( x_attivo<min(xpuntoa, xpuntob) ) continue;
           if( x_attivo>max(xpuntoa, xpuntob) ) continue;
        }
        //----------------------------------------------------------------------------------------
        // Se ho un Agv impostato nel Box di gestione ingombro agv sulla selezione del segmento
        // lo setto per la visualizzazione dell'ingombro di quell'agv
        //----------------------------------------------------------------------------------------
        if(agv_per_ingombro>=1 && agv_per_ingombro<=MAXAGV){
           FormXYNodi->SegmentoPerIngombro[agv_per_ingombro]=i;
           FormXYNodi->CurvaPerIngombro[agv_per_ingombro]=999999;
           FormXYNodi->PuntoPerIngombro[agv_per_ingombro]=999999;
           FormXYNodi->DirezionePerIngombro[agv_per_ingombro]=FormXYNodi->cbDirezioneAgv->ItemIndex;
           FormXYNodi->RotazioneIngombroS[agv_per_ingombro]=atoi(FormXYNodi->meRotazS->Text.c_str());
           FormXYNodi->RotazioneIngombroE[agv_per_ingombro]=atoi(FormXYNodi->meRotazE->Text.c_str());
        }

        break;

     }


     //----------------------------------
     // Selezione singolo nodo
     //----------------------------------
     if(FormNodi->NodoIniziale!=0){
        i = FormXYNodi->CercaNodo(FormNodi->NodoIniziale);
        if(i>0 && i<=MAXPUNTI)  FormXYNodi->DeleteNodi(FormNodi->NodoIniziale);
        else                    FormXYNodi->InsertNodi(FormNodi->NodoIniziale);
        //----------------------------------------------------------------------------------------
        // Se ho un Agv impostato nel Box di gestione ingombro agv sulla selezione del nodo
        // lo setto per la visualizzazione dell'ingombro di quell'agv
        //----------------------------------------------------------------------------------------
        if(agv_per_ingombro>=1 && agv_per_ingombro<=MAXAGV){
           FormXYNodi->SegmentoPerIngombro[agv_per_ingombro]=999999;
           FormXYNodi->CurvaPerIngombro[agv_per_ingombro]=999999;
           FormXYNodi->PuntoPerIngombro[agv_per_ingombro]=FormNodi->NodoIniziale;
           FormXYNodi->DirezionePerIngombro[agv_per_ingombro]=FormXYNodi->cbDirezioneAgv->ItemIndex;
           FormXYNodi->RotazioneIngombroS[agv_per_ingombro]=atoi(FormXYNodi->meRotazS->Text.c_str());
           FormXYNodi->RotazioneIngombroE[agv_per_ingombro]=atoi(FormXYNodi->meRotazE->Text.c_str());
        }

     }
     //----------------------------------
     // Verifica selezione CURVA BEZIER
     //----------------------------------
     else{
        NewCurvaSel=FormXYNodi->CurvaSelezionata;
        for(i=0;;i++){
           if(PercorsoCurvaBezier[i][0]==0 && PercorsoCurvaBezier[i][1]==0 && PercorsoCurvaBezier[i][2]==0) break;
           if(Punti[ PercorsoCurvaBezier[i][1] ][4]==0 || Punti[ PercorsoCurvaBezier[i][2] ][4]==0) continue;
           /*
           //--------------------------------------------------------------------------------------------------
           // Ricavo le coordinate dei tre punti della curva: nodo entrata, nodo uscita e vertice di controllo
           //--------------------------------------------------------------------------------------------------
           xpuntoa = ((Punti[ PercorsoCurvaBezier[i][1] ][0] - offx_attivo)/Rap)*Rid_attivo;
           ypuntoa = ((Punti[ PercorsoCurvaBezier[i][1] ][1] - offy_attivo)/Rap)*Rid_attivo;
           xpuntob = ((Punti[ PercorsoCurvaBezier[i][2] ][0] - offx_attivo)/Rap)*Rid_attivo;
           ypuntob = ((Punti[ PercorsoCurvaBezier[i][2] ][1] - offy_attivo)/Rap)*Rid_attivo;
           xpuntoc = (( PercorsoCurvaBezier[i][3] - offx_attivo)/Rap)*Rid_attivo;
           ypuntoc = (( PercorsoCurvaBezier[i][4] - offy_attivo)/Rap)*Rid_attivo;
           //----------------------------------------------------------------------------------------
           // Algoritmo per controllare se il punto cliccato è all'interno del triangolo della curva
           //----------------------------------------------------------------------------------------
           d1=(x_attivo-xpuntoa)*(ypuntoc-ypuntoa)-(y_attivo-ypuntoa)*(xpuntoc-xpuntoa);
           d2=(xpuntoa-x_attivo)*(ypuntob-ypuntoa)-(ypuntoa-y_attivo)*(xpuntob-xpuntoa);
           d =(xpuntob-xpuntoa)*(ypuntoc-ypuntoa)-(ypuntob-ypuntoa)*(xpuntoc-xpuntoa);
           //----------------------------------------------------------
           // controllo per evitare eccezzioni di diviosione per zero
           //----------------------------------------------------------
           if(d==0) continue;
           a= (double)d1 / (double)d;
           b= (double)d2 / (double)d;
           //-------------------------------------------------------------------------------------
           // Se ho cliccato all'interno del triangolo della curva considero selezionata la curva
           //-------------------------------------------------------------------------------------
           if(a>0 && b>0 && a+b<1){
              //---------------------------------------------------------------------------
              // Un punto può essere all'interno del triangolo formato da più di una curva.
              // Ipotizzando che la curva si sviluppi più o meno nell'intorno dell'asse che
              // congiunge il nodo di entrata e di uscita per capire quale curva ho cercato
              // di selezionare cerco l'asse più vicino al punto che ho cliccato.
              // L'asse è formato dalla conguinzione dei punti Xa,Ya Xb,Yb
              //---------------------------------------------------------------------------
              q=ypuntoa-ypuntob;
              m=(double)(ypuntob-ypuntoa)/(double)(xpuntob-xpuntoa);
              //-----------------------------------------------------------
              // distanza punto retta
              //-----------------------------------------------------------
              new_dist=abs(y_attivo-m*x_attivo-q)/sqrt_algo(1+m*m);
              if(new_dist<old_dist){
                 NewCurvaSel=i;
                 old_dist=new_dist;
              }
           }
           */
           //--------------------------------------------------------------------------------------------------
           // Ricavo le coordinate dei tre punti della curva: nodo entrata, nodo uscita e vertice di controllo
           //--------------------------------------------------------------------------------------------------
           xpuntoa = ((Punti[ PercorsoCurvaBezier[i][1] ][0] - offset[1][0])/Rap)*Rid;
           ypuntoa = ((Punti[ PercorsoCurvaBezier[i][1] ][1] - offset[1][1])/Rap)*Rid;
           xpuntob = ((Punti[ PercorsoCurvaBezier[i][2] ][0] - offset[1][0])/Rap)*Rid;
           ypuntob = ((Punti[ PercorsoCurvaBezier[i][2] ][1] - offset[1][1])/Rap)*Rid;
           xpuntoc = (( PercorsoCurvaBezier[i][3] - offset[1][0])/Rap)*Rid;
           ypuntoc = (( PercorsoCurvaBezier[i][4] - offset[1][1])/Rap)*Rid;
           //-------------------------------------------------------
           // Ricavo i punti di cui ? formata la curva
           //-------------------------------------------------------
           // calcola i coefficienti
           bx = 2 * (xpuntoc - xpuntoa);
           ax = xpuntob - xpuntoa - bx;
           by = 2 * (ypuntoc - ypuntoa);
           ay = ypuntob - ypuntoa - by;
           // cicla tra 0 e 1 con passo step
           t = 0;
           step = 0.01;
           while (t < 1) {
              xt = ax * pow(t, 2) + bx * t + xpuntoa;
              yt = ay * pow(t, 2) + by * t + ypuntoa;

              x = (int) floor(xt + 0.5);
              y = (int) floor(yt + 0.5);
              //------------------------------------------------------------------------
              // Se ho cliccato in un punto appartenente alla curva ( + o - 8 pixel)
              // Seleziono la curva cliccata
              //------------------------------------------------------------------------
              if(x<(x_mouse+5) && x>(x_mouse-5) && y<(y_mouse+5) && y>(y_mouse-5)){
                 NewCurvaSel=i;
                 //------------------------------------------------------------------------
                 // Setto la selezione anche della curva per il disegno dell'ingombro Agv
                 //------------------------------------------------------------------------
                 if(agv_per_ingombro>=1 && agv_per_ingombro<=MAXAGV){
                    FormXYNodi->SegmentoPerIngombro[agv_per_ingombro]=999999;
                    FormXYNodi->PuntoPerIngombro[agv_per_ingombro]=999999;
                    FormXYNodi->CurvaPerIngombro[agv_per_ingombro]=NewCurvaSel;
                    FormXYNodi->DirezionePerIngombro[agv_per_ingombro]=FormXYNodi->cbDirezioneAgv->ItemIndex;
                    FormXYNodi->RotazioneIngombroS[agv_per_ingombro]=atoi(FormXYNodi->meRotazS->Text.c_str());
                    FormXYNodi->RotazioneIngombroE[agv_per_ingombro]=atoi(FormXYNodi->meRotazE->Text.c_str());
                 }
                 break;
              }
              t += step;
           }
        }
        if(NewCurvaSel!=FormXYNodi->CurvaSelezionata){
           FormXYNodi->CurvaSelezionata=NewCurvaSel;
        }
        else{
           FormXYNodi->CurvaSelezionata=9999;
        }
     }
     for(i=1;i<=MAXAGV;i++){
        if(FormXYNodi->SegmentoPerIngombro[agv_per_ingombro] == 999999 &&
           FormXYNodi->PuntoPerIngombro[agv_per_ingombro]    == 999999 &&
           FormXYNodi->CurvaPerIngombro[agv_per_ingombro]    == 999999    ){
           IngombroAgvGrafico[i]->AzzeraIngombroAgv();
        }
     }

     //----------------------------------
     // Aggiornamento finestra
     //----------------------------------
     FormXYNodi->Refresh(FALSE);
  }

  //----------------------------------------------
  // Zoom
  //----------------------------------------------
  if(Button==mbRight){
     //---------------------------------------------
     // Seleziona X e Y per Zoom dinamico con Mouse
     //---------------------------------------------
     ZoomSel=1;
     ZoomX1=ZoomX2=X;
     ZoomY1=ZoomY2=Y;
     //-------------------------------------------
     // MODIFICA COORDINATE NODI
     //-------------------------------------------
     if(!ModificaCoordinateNodi) AbilitaZoom = true;
     //-------------------------------------------
     // GESTIONE SVUOTAMENTO MANUALE MAGAZZINI
     //-------------------------------------------
     if(FormMagaz->NumeroMagazzino>0 && FormMagaz->NumeroMagazzino<=TOTMAG && AbilitaSvuotamentoManualeMag!=0 && TestPercorsiTecnoFerrari==false){
        //--------------------------------------------
        // Se sono in procedura di cancellazione
        // disattivo lo Zoom dinamico
        //--------------------------------------------
        AbilitaZoom=false;
/*
        //====================================================================
        // Per TEST (INSERIMENTO QUOTE IPOTETICHE PASSATE DA PLC TGV)
        //
        //
        int pos;
        short int QuotaP, QuotaH;
        short int TypPresaPLC, PosizionePLC, PianoPLC;
        AnsiString InputStringa;

        InputStringa = InputBox("Introduzione manuale posizioni TGV", "profondità\naltezza forche\n\nformato richiesto (xxxx,yyyy):", "");
        if(InputStringa.IsEmpty()==false){
           pos = InputStringa.Pos(",");
           if(pos!=0){
              QuotaP  = StrToInt(InputStringa.SubString(0,                    pos-1 ));
              QuotaH  = StrToInt(InputStringa.SubString(pos+1, InputStringa.Length()));
           }
           else{
              QuotaP  = StrToInt(InputStringa);
              QuotaH  = 0;
           }
        }
        //--------------------------------------------------------------------
        // Conversione QUOTE in POS/PIANI
        //--------------------------------------------------------------------
        M.ConvertiQuotePLC(FALSE, FormMagaz->NumeroMagazzino, AGV[1]->mission.RapEncoder, AGV[1]->mission.BoxB[0].TipoBox, QuotaP, QuotaH, 0, N_FORCA_B, FormMagaz->NumeroPosizione, 0, &TypPresaPLC, &PosizionePLC, &PianoPLC, buffer);
        sprintf(buffer, "PosREALE:%d, PosCALC:%d", FormMagaz->NumeroPosizione, PosizionePLC);
        ShowMessage(buffer);
        //
        //====================================================================
*/
        //--------------------------------------------
        // MODALITA' SVUOTAMENTO MAG PIENI
        //--------------------------------------------
        svuota_mag=false;
        if(M.m[FormMagaz->NumeroMagazzino]->Assegnazione!=PRES_VUOTO){
           svuota_mag=true;
           //------------------------------------------------
           // Verifica magazzini accoppiati
           //------------------------------------------------
           NumMagAccoppiato=0;
           for(i=0;;i++){
              if(AccoppiaMagaz[i][0]==0 && AccoppiaMagaz[i][1]==0) break;
              if(AccoppiaMagaz[i][0]!=FormMagaz->NumeroMagazzino && AccoppiaMagaz[i][1]!=FormMagaz->NumeroMagazzino) continue;
              if(AccoppiaMagaz[i][0]==FormMagaz->NumeroMagazzino){NumMagAccoppiato=AccoppiaMagaz[i][1]; break;}
              if(AccoppiaMagaz[i][1]==FormMagaz->NumeroMagazzino){NumMagAccoppiato=AccoppiaMagaz[i][0]; break;}
           }
           //----------------------------------------------
           // SVUOTAMENTO MANUALE
           //----------------------------------------------
           if(AbilitaSvuotamentoManualeMag==1){
              FormGrpDest->NumMagazzino=FormMagaz->NumeroMagazzino;
              if(FormGrpDest->ShowModal()==mrOk){
                 //------------------------------------------------
                 // Comando di svuotamento (ritorno Group=0)
                 // identico agli altri svuotamenti
                 //------------------------------------------------
                 if(FormGrpDest->Group==NN){
                    //----------------------------------------------------------------
                    // Andrea 16/05/2018
                    // Traccia su LOG OPERATORE
                    //----------------------------------------------------------------
                    sprintf(buffer, "(LAYOUT ) - (Mag:%d) Empty Store", FormMagaz->NumeroMagazzino);
                    FileLogOperatore(buffer);
                    //------------------------------------------------
                    // Reset della struttura
                    //------------------------------------------------
                    M.m[FormMagaz->NumeroMagazzino]->Stato      = MG_ST_FREE;
                    M.m[FormMagaz->NumeroMagazzino]->NumCestoni = 0;
                    M.m[FormMagaz->NumeroMagazzino]->Completo   = false;
                    for(i=1; i<=MAXTERRAMAG; i++){
                       for(j=0; j<MAXPIANIMAG; j++){
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].Presenza[j] = false;
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].QuotaH[j]   = 0;
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].QuotaP[j]   = 0;
                          memset(&M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j], 0, sizeof(BOX));
                       }
                    }
                    //--------------------------------------------------------------------
                    // MAGAZZINI TEMPORANEI
                    // A svuotamento completato e gestione spotamento con ritorno attiva,
                    // i magazzini devono ritornare per tutti i tipi di prodotto
                    //--------------------------------------------------------------------
                    if(AbilitaSvuotamentoManualeMag==3 && M.m[FormMagaz->NumeroMagazzino]->Tipo==MG_TEMPORANEO){
                       M.m[FormMagaz->NumeroMagazzino]->Tipo=MG_PROD_TUTTI;
                    }
                    //------------------------------------------------
                    // GESTIONE MAGAZZINI ACCOPPIATI
                    // Magazzini accoppiati devono viaggiare i pari
                    // passo
                    //------------------------------------------------
                    if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
                       //------------------------------------------------
                       // Reset della struttura
                       //------------------------------------------------
                       M.m[NumMagAccoppiato]->Stato      = MG_ST_FREE;
                       M.m[NumMagAccoppiato]->NumCestoni = 0;
                       M.m[NumMagAccoppiato]->Completo   = false;
                       for(i=1; i<=MAXTERRAMAG; i++){
                          for(j=0; j<MAXPIANIMAG; j++){
                             M.m[NumMagAccoppiato]->Pos[i].Presenza[j] = false;
                             M.m[NumMagAccoppiato]->Pos[i].QuotaH[j]   = 0;
                             M.m[NumMagAccoppiato]->Pos[i].QuotaP[j]   = 0;
                             memset(&M.m[NumMagAccoppiato]->Pos[i].Box[j], 0, sizeof(BOX));
                          }
                       }
                       //--------------------------------------------------------------------
                       // MAGAZZINI TEMPORANEI
                       // A svuotamento completato e gestione spotamento con ritorno attiva,
                       // i magazzini devono ritornare per tutti i tipi di prodotto
                       //--------------------------------------------------------------------
                       if(AbilitaSvuotamentoManualeMag==3 && M.m[NumMagAccoppiato]->Tipo==MG_TEMPORANEO){
                          M.m[NumMagAccoppiato]->Tipo=MG_PROD_TUTTI;
                       }
                    }
                 }
                 //------------------------------------------------
                 // Comando di abilitazione / diabilitazione Mag
                 //------------------------------------------------
                 else{
                    //----------------------------------------------------------------
                    // Traccia su LOG OPERATORE
                    //----------------------------------------------------------------
                    if(FormGrpDest->Group==1) sprintf(buffer, "(LAYOUT ) - (Mag:%d) Disable Store Group", FormMagaz->NumeroMagazzino );
                    else                      sprintf(buffer, "(LAYOUT ) - (Mag:%d) Enable Store Group", FormMagaz->NumeroMagazzino );
                    FileLogOperatore(buffer);
                    //-------------------------------------------------------
                    // recupero gruppo + abilita/disabilita i mag del gruppo
                    //-------------------------------------------------------
                    grpmag = M.m[FormMagaz->NumeroMagazzino]->Gruppo;
                    for(i=1; i<=MAXMAG; i++){
                       if( M.m[i]->Num>=1 && M.m[i]->Num<=MAXMAG ){
                          if( M.m[i]->Gruppo==grpmag ){
                             prevstato = M.m[i]->Tipo;
                             if( FormGrpDest->Group==1 ) M.m[i]->Tipo = MG_ESCLUSO;
                             if( FormGrpDest->Group==2 ) M.m[i]->Tipo = MG_PROD_TUTTI;
                             if( prevstato != M.m[i]->Tipo ){
                                //---------------------------------------------
                                // Salva Mag[i] se variato
                                //---------------------------------------------
                                err = M.save_record_mag( M.m[i]->Num, buffer);
                                if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
                                else{
                                   //----------------------------------------------------------------
                                   //  Trasmetti comando al SERVER  (Andrea)
                                   //----------------------------------------------------------------
                                   if( IdPC>1 ){
                                      err = RemSincronizzaTimeFile( M.mag_name_sorg, M.mag_name_dest, buffer );
                                      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
                                      sprintf( NomeFile, "Mag%03d", M.m[i]->Num );
                                      err = RemCreaFileComando ( NomeFile, (BYTE *) M.m[i], sizeof( MAG ), buffer );
                                      if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
                                   }
                                }
                             }
                          }
                       }
                    }
                 }
              }
           }
           //----------------------------------------------
           // SPOSTAMENTO PALLET MANUALE
           //----------------------------------------------
           if( AbilitaSvuotamentoManualeMag>1 ){
              FormGrpDest->NumMagazzino=FormMagaz->NumeroMagazzino;
              if(FormGrpDest->ShowModal()==mrOk){
                 //------------------------------------------------
                 // Comando di svuotamento (ritorno Group=0)
                 // identico agli altri svuotamenti
                 //------------------------------------------------
                 if(FormGrpDest->Group==NN){
                    //----------------------------------------------------------------
                    // Andrea 16/05/2018
                    // Traccia su LOG OPERATORE
                    //----------------------------------------------------------------
                    sprintf(buffer, "(LAYOUT ) - (Mag:%d) Empty Store", FormMagaz->NumeroMagazzino);
                    FileLogOperatore(buffer);
                    //------------------------------------------------
                    // Reset della struttura
                    //------------------------------------------------
                    M.m[FormMagaz->NumeroMagazzino]->Stato      = MG_ST_FREE;
                    M.m[FormMagaz->NumeroMagazzino]->NumCestoni = 0;
                    M.m[FormMagaz->NumeroMagazzino]->Completo   = false;
                    for(i=1; i<=MAXTERRAMAG; i++){
                       for(j=0; j<MAXPIANIMAG; j++){
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].Presenza[j] = false;
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].QuotaH[j]   = 0;
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].QuotaP[j]   = 0;
                          memset(&M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j], 0, sizeof(BOX));
                       }
                    }
                    //------------------------------------------------
                    // Magazzini temporanei tronano magazzini per i
                    // pieni
                    //------------------------------------------------
                    if(M.m[FormMagaz->NumeroMagazzino]->Tipo==MG_TEMPORANEO) M.m[FormMagaz->NumeroMagazzino]->Tipo=MG_PROD_TUTTI;
                    //------------------------------------------------
                    // GESTIONE MAGAZZINI ACCOPPIATI
                    // Magazzini accoppiati devono viaggiare i pari
                    // passo
                    //------------------------------------------------
                    if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
                       //------------------------------------------------
                       // Reset della struttura
                       //------------------------------------------------
                       M.m[NumMagAccoppiato]->Stato      = MG_ST_FREE;
                       M.m[NumMagAccoppiato]->NumCestoni = 0;
                       M.m[NumMagAccoppiato]->Completo   = false;
                       for(i=1; i<=MAXTERRAMAG; i++){
                          for(j=0; j<MAXPIANIMAG; j++){
                             M.m[NumMagAccoppiato]->Pos[i].Presenza[j] = false;
                             M.m[NumMagAccoppiato]->Pos[i].QuotaH[j]   = 0;
                             M.m[NumMagAccoppiato]->Pos[i].QuotaP[j]   = 0;
                             memset(&M.m[NumMagAccoppiato]->Pos[i].Box[j], 0, sizeof(BOX));
                          }
                       }
                       //------------------------------------------------
                       // Magazzini temporanei tronano magazzini per i
                       // pieni
                       //------------------------------------------------
                       if(M.m[NumMagAccoppiato]->Tipo==MG_TEMPORANEO) M.m[NumMagAccoppiato]->Tipo=MG_PROD_TUTTI;
                    }
                 }
                 //------------------------------------------------
                 // Comando di abilitazione / diabilitazione Mag
                 //------------------------------------------------
                 if(FormGrpDest->Group==1 || FormGrpDest->Group==2){
                    //----------------------------------------------------------------
                    // Traccia su LOG OPERATORE
                    //----------------------------------------------------------------
                    if(FormGrpDest->Group==1) sprintf(buffer, "(LAYOUT ) - (Mag:%d) Disable Store Group", FormMagaz->NumeroMagazzino );
                    else                      sprintf(buffer, "(LAYOUT ) - (Mag:%d) Enable Store Group", FormMagaz->NumeroMagazzino );
                    FileLogOperatore(buffer);
                    //-------------------------------------------------------
                    // recupero gruppo + abilita/disabilita i mag del gruppo
                    //-------------------------------------------------------
                    grpmag = M.m[FormMagaz->NumeroMagazzino]->Gruppo;
                    for(i=1; i<=MAXMAG; i++){
                       if( M.m[i]->Num>=1 && M.m[i]->Num<=MAXMAG ){
                          if( M.m[i]->Gruppo==grpmag ){
                             prevstato = M.m[i]->Tipo;
                             if( FormGrpDest->Group==1 ) M.m[i]->Tipo = MG_ESCLUSO;
                             if( FormGrpDest->Group==2 ) M.m[i]->Tipo = MG_PROD_TUTTI;
                             if( prevstato != M.m[i]->Tipo ){
                                //---------------------------------------------
                                // Salva Mag[i] se variato
                                //---------------------------------------------
                                err = M.save_record_mag( M.m[i]->Num, buffer);
                                if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
                             }
                          }
                       }
                    }
                 }
                 //------------------------------------------------
                 // Comando di cambio gruppo di destinazione
                 //------------------------------------------------
                 if(FormGrpDest->Group>2){
                    //----------------------------------------------------------------
                    // Verifico se sono state specificate delle posizini ben precise
                    //----------------------------------------------------------------
                    elenco_pos=false;
                    for(i=0; i<(MAXTERRAMAG*MAXPIANIMAG); i++){
                       if(FormGrpDest->PosToMove[i].IsEmpty()) continue;
                       elenco_pos=true;
                    }
                    //----------------------------------------------------------------
                    // Andrea 16/05/2018
                    // Traccia su LOG OPERATORE
                    //----------------------------------------------------------------
                    sprintf(buffer, "(LAYOUT ) - (Mag:%d) Move Store in Group %d (qty:%d, check:%d)", FormMagaz->NumeroMagazzino, FormGrpDest->Group-2, FormGrpDest->QtyPrel, elenco_pos);
                    FileLogOperatore(buffer);
                    //----------------------------------------------------------------
                    // 1. SPOSTAMENTO A QUANTITA'
                    //----------------------------------------------------------------
                    if(elenco_pos==false){
                       qty=0;
                       if( M.m[FormMagaz->NumeroMagazzino]->Fifo==false ){
                          for(i=1; i<=MAXTERRAMAG; i++){
                             for(j=MAXPIANIMAG-1; j>=0; j--){
                                if(M.m[FormMagaz->NumeroMagazzino]->Pos[i].Presenza[j]==false) continue;
                                M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].GruppoDest = FormGrpDest->Group-2;
                                M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].TimeS      = time(NULL);
                                qty++;
                                if( FormGrpDest->QtyPrel>0 && qty>=FormGrpDest->QtyPrel ) break;
                             }
                             if( FormGrpDest->QtyPrel>0 && qty>=FormGrpDest->QtyPrel ) break;
                          }
                       }
                       else{
                          for(i=MAXTERRAMAG; i>0; i--){
                             for(j=MAXPIANIMAG-1; j>=0; j--){
                                if(M.m[FormMagaz->NumeroMagazzino]->Pos[i].Presenza[j]==false) continue;
                                M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].GruppoDest = FormGrpDest->Group-2;
                                M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].TimeS      = time(NULL);
                                qty++;
                                if( FormGrpDest->QtyPrel>0 && qty>=FormGrpDest->QtyPrel ) break;
                             }
                             if( FormGrpDest->QtyPrel>0 && qty>=FormGrpDest->QtyPrel ) break;
                          }
                       }
                    }
                    //----------------------------------------------------------------
                    // 2. SPOSTAMENTO A POSIZIONE
                    //----------------------------------------------------------------
                    else{
                       for(z=0; z<(MAXTERRAMAG*MAXPIANIMAG); z++){
                          if(FormGrpDest->PosToMove[z].IsEmpty()) break;
                          pos = FormGrpDest->PosToMove[z].Pos(",");
                          i = (short int)(atoi(FormGrpDest->PosToMove[z].SubString(0,     pos-1).c_str()));
                          j = (short int)(atoi(FormGrpDest->PosToMove[z].SubString(pos+1, FormGrpDest->PosToMove[z].Length()).c_str()));
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].GruppoDest = FormGrpDest->Group-2;
                          M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].TimeS      = time(NULL);
                       }
                    }
                 }
              }
           }
        }
        //--------------------------------------------
        // MODALITA' RIEMPIMENTO MAG VUOTI
        //--------------------------------------------
        if(M.m[FormMagaz->NumeroMagazzino]->Assegnazione==PRES_VUOTO){
           //--------------------------------------------
           // Richiesta conferma operazione
           //--------------------------------------------
           result = Application->MessageBox(MESS[11], MESS[10], MB_YESNO | MB_ICONQUESTION);
           if(result != IDYES) return;

           //§§§
           //----------------------------------------------------------------
           // Andrea 16/05/2018
           // Traccia su LOG OPERATORE
           //----------------------------------------------------------------
           sprintf(buffer, "(LAYOUT ) - (Mag:%d) Fill Store", FormMagaz->NumeroMagazzino);
           FileLogOperatore(buffer);
           //§§§

           //------------------------------------------------
           // Reset della struttura
           //------------------------------------------------
           M.m[FormMagaz->NumeroMagazzino]->Completo=true;
           M.m[FormMagaz->NumeroMagazzino]->NumCestoni=0;
           for(i=1; i<=M.m[FormMagaz->NumeroMagazzino]->Npt; i++){
              for(j=0; j<=M.m[FormMagaz->NumeroMagazzino]->Sovrapposizione; j++){
                 //--------------------------------------------
                 // Presenza attiva
                 //--------------------------------------------
                 M.m[FormMagaz->NumeroMagazzino]->NumCestoni++;
                 M.m[FormMagaz->NumeroMagazzino]->Pos[i].Presenza[j]=true;
                 //--------------------------------------------
                 // Assegnazione del TIPO BOX a tutte le pos
                 //--------------------------------------------
                 M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].TipoBox = M.m[FormMagaz->NumeroMagazzino]->TipoBox;
                 for(z=0; z<Magaz[FormMagaz->NumeroMagazzino][2]; z++) M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].Presa[z][0]=true;
                 M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].StatoLav=ST_BOX_VUOTI;
                 M.m[FormMagaz->NumeroMagazzino]->Pos[i].Box[j].Codice=ASS_VUOTO;
                 M.m[FormMagaz->NumeroMagazzino]->Stato = MG_ST_VUOTI;
                 //--------------------------------------------------------
                 // Andrea 07/06/2018
                 // Se magazzino era disattivato lascia invariato
                 //--------------------------------------------------------
                 // M.m[FormMagaz->NumeroMagazzino]->Tipo  = MG_VUOTI;
                 if( M.m[FormMagaz->NumeroMagazzino]->Tipo!=MG_ESCLUSO ) M.m[FormMagaz->NumeroMagazzino]->Tipo  = MG_VUOTI;
              }
           }
        }
        //----------------------------------------------------------------
        // Salvataggio su file di tutte le variazioni
        //----------------------------------------------------------------
        err = M.save_record_mag( FormMagaz->NumeroMagazzino, buffer);
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
           err = M.save_record_mag(NumMagAccoppiato, buffer);
           if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        }
        //----------------------------------------------------------------
        // GESTIONE UDC
        // da stuttura software ad array UDC per aggiornamento SQL
        //
        // !!! IMPORTANTE !!!
        // dopo aver aggiornato la struttura globale del software
        //----------------------------------------------------------------
        if(GestioneUdc>0 && svuota_mag==true){
           // -- SET LOCAZIONI SU UDC A BORDO TGV
           if(!err) err = B.StructToUdcTmp(CMD_WR_SQL_UDC_RST_LOC, 0, 0, FormMagaz->NumeroMagazzino, 0, UDC_ALL_STZ, UDC_ALL_POS, UDC_ALL_LEV, N_FORCA_AB, buffer);
           if(GestioneUdc==2 && err!=0) err=0;
           if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
        }

        //################################################################
        //----------------------------------------------------------------
        //  Trasmetti comando al SERVER  (Andrea)
        //----------------------------------------------------------------
        if( IdPC>1 ){
           err = RemSincronizzaTimeFile( M.mag_name_sorg, M.mag_name_dest, buffer );
           if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
           sprintf( NomeFile, "Mag%03d", FormMagaz->NumeroMagazzino );
           err = RemCreaFileComando ( NomeFile, (BYTE *) M.m[ FormMagaz->NumeroMagazzino ], sizeof( MAG ), buffer );
           if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
           if(NumMagAccoppiato>0 && NumMagAccoppiato<=TOTMAG){
              sprintf( NomeFile, "Mag%03d", NumMagAccoppiato );
              err = RemCreaFileComando ( NomeFile, (BYTE *) M.m[ NumMagAccoppiato ], sizeof( MAG ), buffer );
              if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
           }
        }
        //################################################################

     }
  }
  //----------------------------------------------
  // Spostamento Layout
  //----------------------------------------------
  if(Button==mbLeft && AbilitaZoom==false && !FormNodi->NodoIniziale){
     SpostManAttivo = true;
     SpostManX = X;
     SpostManY = Y;
     //---------------------------------
     // Visualizza Quote in cm
     //---------------------------------
     x = ((SpostManX/Rid)*Rap)+offset[1][0];
     y = ((SpostManY/Rid)*Rap)+offset[1][1];
  }
  //----------------------------------------------
  // Gestione SFONDO GRAFICO
  //----------------------------------------------
  if(Button==mbLeft && AbilitaSfondoGrafico==true){
     SpostManX = X;
     SpostManY = Y;
     //---------------------------------
     // Visualizza Quote in cm
     //---------------------------------
     x = ((SpostManX/Rid)*Rap)+offset[1][0];
     y = ((SpostManY/Rid)*Rap)+offset[1][1];
// riattiva SfondoGrafico
   //COMISfondo->MouseLeft(x, y);
  }
  //----------------------------------------------
  // Gestione SFONDO GRAFICO
  //----------------------------------------------
  if(Button==mbRight && AbilitaSfondoGrafico==true){
     SpostManX = X;
     SpostManY = Y;
     //---------------------------------
     // Visualizza Quote in cm
     //---------------------------------
     x = ((SpostManX/Rid)*Rap)+offset[1][0];
     y = ((SpostManY/Rid)*Rap)+offset[1][1];
// riattiva SfondoGrafico
   //COMISfondo->MouseRight(x, y);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  bool trovato;
  char buffer[2301]="";
  char buffer1[501]="";
  char buffer2[101]="";
  int i, j, g, s, k, h, z, w, Xdifcm, Ydifcm;
  int Xatt, Yatt, Xdif, Ydif;
  int xcen, ycen, xpal, ypal, mezzo;
  int x, y, x_mouse, y_mouse, grd, tot_pal, tot_pal_mag;
  int yic, yfc, xic, xfc, x_max, y_max, x_min, y_min;
  int num_nod, nodo_p, sviluppo, posti, offs1;
  int xinc1, xinc2, xinc3, xinc4;
  int yinc1, yinc2, yinc3, yinc4;
  int larg_cest = LARG_CEST;
  int lung_cest = LUNG_CEST;
  int carico_in_punta  = CARICO_IN_PUNTA;
  int dist_tra_palette = DIST_CEST_IN_MAG;
  int x_attivo, y_attivo, offx_attivo, offy_attivo;
  float Rid_attivo;
  short int elenco_baie[TOTBAIE+1];
  short int num_mac1, tipo_box, offset_da_nodo;
  long double offsX_nod, offsY_nod, offsX, offsY, hip;
  long double sinA, cosA, sinB, cosB, sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double radA_m, radA, radB, radC, radB1, radB2;
  long double ComponenteXOffset, ComponenteYOffset;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  char stato_pallet[21]="";

  x_mouse=X;
  y_mouse=Y;
  FormLayout->Hint="";
  FormLayout->SetFocus();
  memset(&elenco_baie[0], 0, sizeof(elenco_baie));
  Screen->Cursor=crDefault;
  //--------------------------------------------------------
  // Blocco i controlli del mouse fino a che la pagina è
  // in apertura
  //--------------------------------------------------------
  if(FormMain->FormInOpen==true) return;
  //--------------------------------------------------------
  // Gestione dello ZOOM
  //--------------------------------------------------------
  if(AbilitaZoom==true || ModificaCoordinateNodi==true){
     //---------------------------------------------
     // Se attiva selezione Zoom dinamica con mouse
     // allora recupera X2,Y2 per disegno riquadro
     //---------------------------------------------
     if( ZoomSel==1){
        ZoomX2=X;
        ZoomY2=Y;   // - Offset di visualizzazione sfondo
     }
  }
  //-------------------------------------------
  // Spostamento Layout
  //-------------------------------------------
  if(SpostManAttivo==true){
     //-----------------------------------------------------------------------
     // Trovo la X e la Y attuale e la differenza rispetto a quella iniziale
     //-----------------------------------------------------------------------
     Xatt = X;
     Yatt = Y;
     Xdif = Xatt-SpostManX;
     Ydif = Yatt-SpostManY;
     //---------------------------------------------------
     // calcolo la differenza in cm  (/Rip*RAP)
     //---------------------------------------------------
     Xdifcm = (Xdif/Rid)*Rap;
     Ydifcm = (Ydif/Rid)*Rap;
     offset[1][0] = offset[1][0] - Xdifcm;
     offset[1][1] = offset[1][1] - Ydifcm;
     //-------------------------------------------
     // salvo nuova posizione mouse
     //-------------------------------------------
     SpostManX = Xatt;
     SpostManY = Yatt;
  }

  //-----------------------------------------------------
  // Memorizzo parametri di puntamento del Mouse
  //-----------------------------------------------------
  offx_attivo = offset[1][0];
  offy_attivo = offset[1][1];
  Rid_attivo  = Rid;
  x_attivo    = x_mouse;
  y_attivo    = y_mouse;

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // di una *** MACCHINA ***
  //-----------------------------------------------------
  trovato = false;
  //-------------------------------------------
  // Disegno del Palettizzatore come insieme
  // di Stazioni
  //-------------------------------------------
  for(i=1; i<=TOTCHIAMATE; i++){                // per tutte le macchine
     //-------------------------------------------------------
     // Già trovato qualcosa --> skip!
     //-------------------------------------------------------
     if(trovato==true            ) break;
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(P.ch[i]->Num<1           ) continue;
     if(P.ch[i]->Num>TOTCHIAMATE ) continue;
     //-------------------------------------------------------
     // Se è abilitata la visualizz. zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Recupero dati legati alla singola macchina
     //-------------------------------------------------------
     lung_cest  = LUNG_CEST;
     sviluppo = Macch[i][0];                    // Direzione di sviluppo (direzione da
                                                // primo cestone depositato -> a nodo)
     //-------------------------------------------------------
     // Ciclo per tutte le stazioni
     //-------------------------------------------------------
     for(s=0; s<MAXSTAZMAC; s++){                // per tutte le stazioni
        //-------------------------------------------------------
        // dati
        //-------------------------------------------------------
        num_nod  = P.ch[i]->Staz[s].PuntoIng;     // Numero del nodo di imgresso magazzino
        posti    = P.ch[i]->Staz[s].Npt;          // Posti previsti su stazione
        if(num_nod==0) continue;                  // Se il Nodo del Magazzino=0 ---> Skip!
        //-------------------------------------------------------
        // Locazione inesistente  (posti=0)
        //-------------------------------------------------------
        if(posti==0  ) continue;

        //-------------------------------------------------------
        // 30/04/97 Disegna con l'offset relativo alla pagina
        //-------------------------------------------------------
        xcen = ((Punti[num_nod][0]-offx_attivo));            // pixel x del nodo occupato
        ycen = ((Punti[num_nod][1]-offy_attivo));            // pixel y del nodo occupato

        //-------------------------------------------------------
        // Recupero i gradi di rotazione in base allo sviluppo
        // assegnato
        //-------------------------------------------------------
        grd = sviluppo;
        //-------------------------------------------------------
        // Aggiustamento dello sviluppo standard ai gradi
        //-------------------------------------------------------
        if(sviluppo==0) grd=0;
        if(sviluppo==1) grd=180;
        if(sviluppo==2) grd=90;
        if(sviluppo==3) grd=270;

        //-------------------------------------------------------
        // Gradi to Radianti
        //-------------------------------------------------------
        radA_m = DegToRad(grd);
        //-------------------------------------------------------
        // Calcolo delle dimensioni grafiche della paletta
        //-------------------------------------------------------
        MezzoLatoX = (long double)(larg_cest/2);
        MezzoLatoY = (long double)(lung_cest/2);
        //-------------------------------------------------------
        // Inizilizzo offSet di posizionamento PALETTA
        //-------------------------------------------------------
        offsX_nod = 0;
        offsY_nod = 0;  // inizializzazione con offset manuale
        if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
        else{
           offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
        }
        //-------------------------------------------------------
        // Disegno di tutte le postazioni
        //-------------------------------------------------------
        for(j=1; j<=posti; j++){
           //-------------------------------------------------------
           // Incremento offset posizione TGV da nodo
           //-------------------------------------------------------
           offsX = offsX_nod;
           offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette));
           //--------------------------------------
           // Recupero gradi singola stazione
           //--------------------------------------
           radA = radA_m;
           if(P.ch[i]->Staz[s].RotLayout!=0){
              radA = DegToRad(P.ch[i]->Staz[s].RotLayout);
           }
           //--------------------------------------
           // Visualizzazione tanti riquadri
           // quante le tot posizini in presa
           //--------------------------------------
           for(z=0; z<Macch[i][2]; z++){
              //-------------------------------------------------------
              // Calcolo coordinate grafiche del centro paletta
              //-------------------------------------------------------
              offsX = (((larg_cest * (Macch[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
              //-------------------------------------------------------
              // ipotenusa del triangolo rettangolo i cui lati sono
              // gli offset del centro paletta dal centro navetta
              //-------------------------------------------------------
              radB  = (long double)(atanl( offsX / offsY ));
              SinCos(radB, sinB, cosB);
              hip   = offsY / cosB;
              //-------------------------------------------------------
              // Calcolo nell'angolo di rotazione del centro paletta
              // come sommatoria dei gradi del TGV + l'angolo di
              // rotazione dal centro TGV al centro paletta
              //-------------------------------------------------------
              radC = radA + radB;
              SinCos(radC, sinC, cosC);
              //-------------------------------------------------------
              // Offset in X,Y del centro PALLET rispetto al centro TGV
              //-------------------------------------------------------
              ComponenteYOffset = hip * cosC;
              ComponenteXOffset = hip * sinC;

              xpal = xcen-(int)(ComponenteXOffset);
              ypal = ycen-(int)(ComponenteYOffset);
              //-------------------------------------------------------
              // Recupero coordinate vertici box per visualizzazione
              //-------------------------------------------------------
              radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

              radB1 = radA + radB;
              radB2 = radA - radB;
              SinCos(radB1, sinB1, cosB1);
              SinCos(radB2, sinB2, cosB2);

              MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

              ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
              ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
              ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
              ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

              xinc1 = (int) (xpal-(int)(ComponenteX1));
              xinc2 = (int) (xpal+(int)(ComponenteX2));
              xinc3 = (int) (xpal+(int)(ComponenteX1));
              xinc4 = (int) (xpal-(int)(ComponenteX2));

              yinc1 = (int) (ypal+(int)(ComponenteY1));
              yinc2 = (int) (ypal-(int)(ComponenteY2));
              yinc3 = (int) (ypal-(int)(ComponenteY1));
              yinc4 = (int) (ypal+(int)(ComponenteY2));

              //-----------------------------------------------
              // Aggiustamenti dovuti ad arrotondamenti
              //-----------------------------------------------
              if(grd==90 || grd==270){
                 offs1 = abs(xinc4-xinc3);
                 if(offs1<3) xinc4=xinc3;
                 offs1 = abs(xinc2-xinc1);
                 if(offs1<3) xinc2=xinc1;
                 offs1 = abs(yinc4-yinc1);
                 if(offs1<3) yinc4=yinc1;
                 offs1 = abs(yinc3-yinc2);
                 if(offs1<3) yinc3=yinc2;
              }
              if(grd==0 || grd==180){
                 offs1 = abs(xinc4-xinc1);
                 if(offs1<3) xinc4=xinc1;
                 offs1 = abs(xinc3-xinc2);
                 if(offs1<3) xinc3=xinc2;
                 offs1 = abs(yinc2-yinc1);
                 if(offs1<3) yinc2=yinc1;
                 offs1 = abs(yinc4-yinc3);
                 if(offs1<3) yinc4=yinc3;
              }

              //------------------------------------------------------
              // Se il mouse viene premuto all'interno dell'area
              // occupata da una macchina allora visualizza la Pagina
              // di editor della macchina
              //------------------------------------------------------
              xic = xinc1/Rap*Rid_attivo;
              xfc = xinc3/Rap*Rid_attivo;
              yic = yinc1/Rap*Rid_attivo;
              yfc = yinc3/Rap*Rid_attivo;
              if(yic > yfc){
                 y_max=  yic;
                 y_min=  yfc;
              }
              else{
                 y_max=  yfc;
                 y_min=  yic;
              }
              if(xic > xfc){
                 x_max=  xic;
                 x_min=  xfc;
              }
              else{
                 x_max=  xfc;
                 x_min=  xic;
              }
              if(( y_attivo > y_min )&&( y_attivo < y_max )){
                 if(( x_attivo > x_min )&&( x_attivo < x_max )){
                    //------------------------------------------
                    // Intestazione dicitura
                    // **** MACCHINA ***
                    //------------------------------------------
                    Screen->Cursor=crHandPoint;
                    sprintf(buffer, "MNr.%d - %s", P.ch[i]->Num, P.ch[i]->Sigla);
                    //-------------------------------
                    // Visualizzazione dati
                    //-------------------------------
                    FormLayout->Hint=buffer;
                    trovato = true;
                    break;
                 }
              }
           }
        }
        if(trovato) break;
     }
     if(trovato) break;
  }

  //-------------------------------------------
  // NON SCAFFALI
  // Ordino le baie per piano di appartenenza
  //-------------------------------------------
  k=0;
  for(i=1; i<=TOTBAIE; i++){                // per tutte le baie
     if(P.ba[i]->Num<1       ) continue;
     if(P.ba[i]->Num>TOTBAIE ) continue;
     elenco_baie[k] = i;
     k++;
  }

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // di una *** BAIE ***
  //-----------------------------------------------------
  for(g=0; g<TOTBAIE; g++){                // per tutte le baie
     //-------------------------------------------------------
     // Indice baia considerata (recuperata da elenco)
     //-------------------------------------------------------
     i = elenco_baie[g];
     if(i==0) break;
     //-------------------------------------------------------
     // Già trovato qualcosa --> skip!
     //-------------------------------------------------------
     if(trovato==true               ) break;
     //-------------------------------------------------------
     // Se è abilitata la visualizz. zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(P.ba[i]->Num<1              ) continue;
     if(P.ba[i]->Num>TOTBAIE        ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     lung_cest  = LUNG_CEST;
     num_nod    = P.ba[i]->Punto;      // Numero del nodo di imgresso magazzino
     sviluppo   = Baie[i][0];          // Direzione di sviluppo (direzione da
                                       // primo cestone depositato -> a nodo)
     posti      = Baie[i][1];          // Massimo numero cestoni
     if(num_nod==0          ) continue;     // Se il Nodo del Magazzino=0 ---> Skip!
     if(Punti[num_nod][4]==0) continue;     // pagina non impostata
     //-------------------------------------------------------
     // Locazione inesistente  (posti=0)
     //-------------------------------------------------------
     if(posti==0) continue;
     //-------------------------------------------------------
     // 30/04/97 Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offx_attivo));            // pixel x del nodo occupato
     ycen = ((Punti[num_nod][1]-offy_attivo));            // pixel y del nodo occupato
     //-------------------------------------------------------
     // Recupero i gradi di rotazione in base allo sviluppo
     // assegnato
     //-------------------------------------------------------
     grd = sviluppo;
     //-------------------------------------------------------
     // Aggiustamento dello sviluppo standard ai gradi
     //-------------------------------------------------------
     if(sviluppo==0) grd=0;
     if(sviluppo==1) grd=180;
     if(sviluppo==2) grd=90;
     if(sviluppo==3) grd=270;
     //-------------------------------------------------------
     // Gradi to Radianti
     //-------------------------------------------------------
     radA = DegToRad(grd);
     //-------------------------------------------------------
     // Calcolo delle dimensioni grafiche della paletta
     //-------------------------------------------------------
     MezzoLatoX = (long double)(larg_cest/2);
     MezzoLatoY = (long double)(lung_cest/2);
     //-------------------------------------------------------
     // Inizilizzo offSet di posizionamento PALETTA
     //-------------------------------------------------------
     offsX_nod = 0;
     offsY_nod = Baie[i][3];  // inizializzazione con offset manuale
     if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
     else{
        offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
     }
     //-------------------------------------------------------
     // Disegno di tutte le postazioni
     //-------------------------------------------------------
     for(j=1; j<=posti; j++){
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsX = offsX_nod;
        offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette));
        //--------------------------------------
        // Visualizzazione tanti riquadri
        // quante le tot posizini in presa
        //--------------------------------------
        for(z=0; z<Baie[i][2]; z++){
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (Baie[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);

           //-------------------------------------------------------
           // Recupero coordinate vertici box per visualizzazione
           //-------------------------------------------------------
           radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           radB1 = radA + radB;
           radB2 = radA - radB;
           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

           xinc1 = (int) (xpal-(int)(ComponenteX1));
           xinc2 = (int) (xpal+(int)(ComponenteX2));
           xinc3 = (int) (xpal+(int)(ComponenteX1));
           xinc4 = (int) (xpal-(int)(ComponenteX2));

           yinc1 = (int) (ypal+(int)(ComponenteY1));
           yinc2 = (int) (ypal-(int)(ComponenteY2));
           yinc3 = (int) (ypal-(int)(ComponenteY1));
           yinc4 = (int) (ypal+(int)(ComponenteY2));

           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xinc4-xinc3);
              if(offs1<3) xinc4=xinc3;
              offs1 = abs(xinc2-xinc1);
              if(offs1<3) xinc2=xinc1;
              offs1 = abs(yinc4-yinc1);
              if(offs1<3) yinc4=yinc1;
              offs1 = abs(yinc3-yinc2);
              if(offs1<3) yinc3=yinc2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xinc4-xinc1);
              if(offs1<3) xinc4=xinc1;
              offs1 = abs(xinc3-xinc2);
              if(offs1<3) xinc3=xinc2;
              offs1 = abs(yinc2-yinc1);
              if(offs1<3) yinc2=yinc1;
              offs1 = abs(yinc4-yinc3);
              if(offs1<3) yinc4=yinc3;
           }

           //------------------------------------------------------
           // Se il mouse viene premuto all'interno dell'area
           // occupata da una macchina allora visualizza la Pagina
           // di editor della macchina
           //------------------------------------------------------
           xic = xinc1/Rap*Rid_attivo;
           xfc = xinc3/Rap*Rid_attivo;
           yic = yinc1/Rap*Rid_attivo;
           yfc = yinc3/Rap*Rid_attivo;
           if(yic > yfc){
              y_max=  yic;
              y_min=  yfc;
           }
           else{
              y_max=  yfc;
              y_min=  yic;
           }
           if(xic > xfc){
              x_max=  xic;
              x_min=  xfc;
           }
           else{
              x_max=  xfc;
              x_min=  xic;
           }
           if(( y_attivo > y_min )&&( y_attivo < y_max )){
              if(( x_attivo > x_min )&&( x_attivo < x_max )){
                 //-----------------------------------------
                 // Intestazione dicitura
                 // **** BAIE ***
                 //-----------------------------------------
                 Screen->Cursor=crHandPoint;
                 if(P.ba[i]->Tipo==BA_NONE     ) sprintf(buffer, "bNr.%d (%s)\n%s\n%s", P.ba[i]->Num, P.ba[i]->Sigla, "BAIA ESCLUSA",   DESCTIPOBOX[P.ba[i]->TipoBox[0]]);
                 if(P.ba[i]->Tipo==BA_DEPOSITO ) sprintf(buffer, "bNr.%d (%s)\n%s\n%s", P.ba[i]->Num, P.ba[i]->Sigla, "BAIA DEPOSITO",  DESCTIPOBOX[P.ba[i]->TipoBox[0]]);
                 if(P.ba[i]->Tipo==BA_PRELIEVO ){
                    sprintf(buffer, "bNr.%d (%s)\n", P.ba[i]->Num, P.ba[i]->Sigla);
                    for(w=0; w<Baie[i][1]; w++){
                       for(k=0; k<Baie[i][2]; k++){
                          tot_pal=0;
                          for(h=0; h<PIANIINPRESA; h++){
                             if(P.ba[i]->Presa[w][k][h]==false) continue;
                             tot_pal++;
                          }
                          // Nessuna presenza
                          if(tot_pal==0){
                             sprintf(buffer1, "(%02d - %03d) %s\n", P.ba[i]->Codice[w], P.ba[i]->Lotto[w], "free");
                          }
                          else{
                             // ALTI
                             if(P.ba[i]->Basso[w]==false) strcpy(stato_pallet, "ALTO");
                             // BASSI
                             else strcpy(stato_pallet, "BASSO");
                             // CODICE
                             if(tot_pal>1) sprintf(buffer1, "(%02d - %03d) (%sx%d)\n", P.ba[i]->Codice[w], P.ba[i]->Lotto[w], stato_pallet, tot_pal);
                             else          sprintf(buffer1, "(%02d - %03d) (%s)", P.ba[i]->Codice[w], P.ba[i]->Lotto[w], stato_pallet);
                          }
                          strcat(buffer, buffer1);
                       }
                    }
                 }
                 //-------------------------------------------------
                 // Visualizzazione dati
                 //-------------------------------------------------
                 FormLayout->Hint=buffer;
                 trovato = true;
                 break;
              }
           }
        }
        if(trovato) break;
     }
     if(trovato) break;
  }

  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // di una *** MAGAZZINI ***
  //-----------------------------------------------------
  for(i=1; i<=TOTMAG; i++){                // per tutte le macchine
     //-------------------------------------------------------
     // Già trovato qualcosa --> skip!
     //-------------------------------------------------------
     if(trovato==true            ) break;
     //-------------------------------------------------------
     // Se è abilitata la visualizz. zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(M.m[i]->Num<1      ) continue;
     if(M.m[i]->Num>TOTMAG ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     num_nod        = M.m[i]->PuntoIng;     // Numero del nodo di imgresso magazzino
     sviluppo       = Magaz[i][0];          // Direzione di sviluppo (direzione da
                                            // primo cestone depositato -> a nodo)
     posti          = M.m[i]->Npt;          // Capacità magazzino
     tipo_box       = M.m[i]->TipoBox;      // Capacità magazzino

     if(num_nod==0          ) continue;     // Se il Nodo del Magazzino=0 ---> Skip!
     if(Punti[num_nod][4]==0) continue;     // pagina non impostata
     //-------------------------------------------------------
     // Locazione inesistente  (posti=0)
     //-------------------------------------------------------
     if(posti==0  ) continue;

     //-------------------------------------------------------
     // Conteggio reale pallet a magazzino
     //-------------------------------------------------------
     tot_pal_mag=0;
     for(j=1; j<=MAXTERRAMAG; j++){
        for(k=0; k<MAXPIANIMAG; k++){
           if(M.m[i]->Pos[j].Presenza[k]==false) continue;
           for(z=0; z<Magaz[i][2]; z++){
              for(h=0; h<PIANIINPRESA; h++){
                 if(M.m[i]->Pos[j].Box[k].Presa[z][h]==false) continue;
                 tot_pal_mag++;
              }
           }
        }
     }

     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     lung_cest         = M.TipiBox[tipo_box].Lung;
     larg_cest         = M.TipiBox[tipo_box].Larg;
     dist_tra_palette  = M.TipiBox[tipo_box].dist_box_in_magaz;
     offset_da_nodo    = M.m[i]->OffSetDaNodo + M.TipiBox[tipo_box].offset_tgv_da_nodo_mag;

     //-------------------------------------------------------
     // 30/04/97 Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offx_attivo));            // pixel x del nodo occupato
     ycen = ((Punti[num_nod][1]-offy_attivo));            // pixel y del nodo occupato
     //-------------------------------------------------------
     // Recupero i gradi di rotazione in base allo sviluppo
     // assegnato
     //-------------------------------------------------------
     grd = sviluppo;
     //-------------------------------------------------------
     // Aggiustamento dello sviluppo standard ai gradi
     //-------------------------------------------------------
     if(sviluppo==0) grd=0;
     if(sviluppo==1) grd=180;
     if(sviluppo==2) grd=90;
     if(sviluppo==3) grd=270;
     //-------------------------------------------------------
     // Gradi to Radianti
     //-------------------------------------------------------
     radA = DegToRad(grd);
     //-------------------------------------------------------
     // Calcolo delle dimensioni grafiche della paletta
     //-------------------------------------------------------
     MezzoLatoX = (long double)(larg_cest/2);
     MezzoLatoY = (long double)(lung_cest/2);
     //-------------------------------------------------------
     // Inizilizzo offSet di posizionamento PALETTA
     //-------------------------------------------------------
     offsX_nod = 0;
     offsY_nod = Magaz[i][1];  // inizializzazione con offset manuale
     if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
     else{
        offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
     }
     //-------------------------------------------------------
     // Disegno di tutte le postazioni
     //-------------------------------------------------------
     for(j=1; j<=posti; j++){
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsX = offsX_nod;
        offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette)) + offset_da_nodo;
        //--------------------------------------
        // Recupero eventuale nodo posizione
        //--------------------------------------
        nodo_p = M.m[i]->Pos[j].NodoIng;
        if(nodo_p>0 && nodo_p<=TOTPUNTI){
           xcen  = ((Punti[nodo_p][0]-offset[1][0]));            // pixel x del nodo occupato
           ycen  = ((Punti[nodo_p][1]-offset[1][1]));            // pixel y del nodo occupato
           offsY = offsY_nod;
        }
        //--------------------------------------
        // Scroll di tutte le posizioni in presa
        //--------------------------------------
        for(z=0; z<Magaz[i][2]; z++){
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (Magaz[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);

           //-------------------------------------------------------
           // Recupero coordinate vertici box per visualizzazione
           //-------------------------------------------------------
           radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           radB1 = radA + radB;
           radB2 = radA - radB;
           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );


           xinc1 = (int) (xpal-(int)(ComponenteX1));
           xinc2 = (int) (xpal+(int)(ComponenteX2));
           xinc3 = (int) (xpal+(int)(ComponenteX1));
           xinc4 = (int) (xpal-(int)(ComponenteX2));

           yinc1 = (int) (ypal+(int)(ComponenteY1));
           yinc2 = (int) (ypal-(int)(ComponenteY2));
           yinc3 = (int) (ypal-(int)(ComponenteY1));
           yinc4 = (int) (ypal+(int)(ComponenteY2));

           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xinc4-xinc3);
              if(offs1<3) xinc4=xinc3;
              offs1 = abs(xinc2-xinc1);
              if(offs1<3) xinc2=xinc1;
              offs1 = abs(yinc4-yinc1);
              if(offs1<3) yinc4=yinc1;
              offs1 = abs(yinc3-yinc2);
              if(offs1<3) yinc3=yinc2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xinc4-xinc1);
              if(offs1<3) xinc4=xinc1;
              offs1 = abs(xinc3-xinc2);
              if(offs1<3) xinc3=xinc2;
              offs1 = abs(yinc2-yinc1);
              if(offs1<3) yinc2=yinc1;
              offs1 = abs(yinc4-yinc3);
              if(offs1<3) yinc4=yinc3;
           }

           //------------------------------------------------------
           // Se il mouse viene premuto all'interno dell'area
           // occupata da una macchina allora visualizza la Pagina
           // di editor della macchina
           //------------------------------------------------------
           xic = xinc1/Rap*Rid_attivo;
           xfc = xinc3/Rap*Rid_attivo;
           yic = yinc1/Rap*Rid_attivo;
           yfc = yinc3/Rap*Rid_attivo;
           if(yic > yfc){
              y_max=  yic;
              y_min=  yfc;
           }
           else{
              y_max=  yfc;
              y_min=  yic;
           }
           if(xic > xfc){
              x_max=  xic;
              x_min=  xfc;
           }
           else{
              x_max=  xfc;
              x_min=  xic;
           }
           if(( y_attivo > y_min )&&( y_attivo < y_max )){
              if(( x_attivo > x_min )&&( x_attivo < x_max )){
                 //------------------------------------------------
                 // DATI MAGAZZINO
                 //------------------------------------------------
                 switch(M.m[i]->Assegnazione){
                    case ASS_DINAMICA: strcpy(buffer1, MESS[518]);
                                       break;
                    case PRES_LAV1:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD1);
                                       else                           strcpy(buffer1, ASS_LAV1);
                                       break;
                    case PRES_LAV2:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD2);
                                       else                           strcpy(buffer1, ASS_LAV2);
                                       break;
                    case PRES_LAV3:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD3);
                                       else                           strcpy(buffer1, ASS_LAV3);
                                       break;
                    case PRES_LAV4:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD4);
                                       else                           strcpy(buffer1, ASS_LAV4);
                                       break;
                    case PRES_LAV5:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD5);
                                       else                           strcpy(buffer1, ASS_LAV5);
                                       break;
                    case PRES_LAV6:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD6);
                                       else                           strcpy(buffer1, ASS_LAV6);
                                       break;
                    case PRES_LAV7:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD7);
                                       else                           strcpy(buffer1, ASS_LAV7);
                                       break;
                    case PRES_LAV8:    if(AbbinaCodiceStatoBox==true) sprintf(buffer1, MESS[529], ASS_COD8);
                                       else                           strcpy(buffer1, ASS_LAV8);
                                       break;
                    case PRES_VUOTO:   strcpy(buffer1, MESS[530]);
                                       break;
                    default:           strcpy(buffer1, "UNKNOWN");
                                       break;
                 }
                 //-----------------------------------------
                 // Intestazione dicitura
                 // **** MAGAZZINI ***
                 //-----------------------------------------
                 Screen->Cursor=crHandPoint;
                 strcpy(buffer1, "");
                 sprintf(buffer, "mNr.%d - %s (tot:%d)\n", M.m[i]->Num, buffer1, tot_pal_mag);
                 if(M.m[i]->Stato==MG_ST_PIENI){
                    if(M.m[i]->Pos[j].Presenza[1]==true){
                       tot_pal=0;
                       for(k=0; k<Magaz[i][2]; k++){
                          for(h=0; h<PIANIINPRESA; h++){
                             if(M.m[i]->Pos[j].Box[1].Presa[k][h]==false) continue;
                             tot_pal++;
                          }
                       }
                       // ALTI
                       if(M.m[i]->Pos[j].Box[1].Basso==false) strcpy(stato_pallet, "ALTO");
                       // BASSI
                       else strcpy(stato_pallet, "BASSO");
                       sprintf(buffer1, "01: Lin:%02d Batch:%03d %4dmm (%sx%d)\n", M.m[i]->Pos[j].Box[1].Codice, M.m[i]->Pos[j].Box[1].Lotto, M.m[i]->Pos[j].Box[1].Altezza, stato_pallet, tot_pal);
                       if(GestioneUdc>0){
                          for( g=0; g<MAXUDCBOX; g++ ){
                             if(strcmp(M.m[i]->Pos[j].Box[1].Prd[g].Udc, "")==0) continue;
                             sprintf(buffer2, "HU:%s, %s, %s\n", M.m[i]->Pos[j].Box[1].Prd[g].Udc, M.m[i]->Pos[j].Box[1].Prd[g].code, M.m[i]->Pos[j].Box[1].Prd[g].description);
                             strcat(buffer1, buffer2);
                          }
                       }
                    }
                    else{
                       strcpy(buffer1, "01: free\n");
                    }
                    strcat(buffer, buffer1);
                    if(M.m[i]->Pos[j].Presenza[0]==true){
                       tot_pal=0;
                       for(k=0; k<Magaz[i][2]; k++){
                          for(h=0; h<PIANIINPRESA; h++){
                             if(M.m[i]->Pos[j].Box[0].Presa[k][h]==false) continue;
                             tot_pal++;
                          }
                       }
                       // ALTI
                       if(M.m[i]->Pos[j].Box[0].Basso==false) strcpy(stato_pallet, "ALTO");
                       // BASSI
                       else strcpy(stato_pallet, "BASSO");
                       sprintf(buffer1, "00: Lin:%02d Batch:%03d %4dmm (%sx%d)\n", M.m[i]->Pos[j].Box[0].Codice, M.m[i]->Pos[j].Box[0].Lotto, M.m[i]->Pos[j].Box[0].Altezza, stato_pallet, tot_pal);
                       if(GestioneUdc>0){
                          for( g=0; g<MAXUDCBOX; g++ ){
                             if(strcmp(M.m[i]->Pos[j].Box[0].Prd[g].Udc, "")==0) continue;
                             sprintf(buffer2, "HU:%s, %s, %s\n", M.m[i]->Pos[j].Box[0].Prd[g].Udc, M.m[i]->Pos[j].Box[0].Prd[g].code, M.m[i]->Pos[j].Box[0].Prd[g].description);
                             strcat(buffer1, buffer2);
                          }
                       }
                    }
                    else{
                       strcpy(buffer1, "00: free\n");
                    }
                    strcat(buffer, buffer1);
                 }
                 //-------------------------------
                 // Visualizzazione dati
                 //-------------------------------
                 FormLayout->Hint=buffer;
                 trovato = true;
                 break;
              }
           }
           if(trovato) break;
        }
        if(trovato) break;
     }
  }
  //-----------------------------------------------------
  // verifica se il mouse è all'interno di un TGV
  // **** TGV ***
  //-----------------------------------------------------
  if(TestPercorsiTecnoFerrari==false){
     mezzo = (long double)(((LUNG_AGV/Rap)*Rid_attivo)/2);
     for(i=1; i<=MAXAGV; i++){
        xic = Pos_Agv[i].xc-mezzo;
        xfc = Pos_Agv[i].xc+mezzo;
        yic = Pos_Agv[i].yc-mezzo;
        yfc = Pos_Agv[i].yc+mezzo;
        //------------------------------------------------------
        // Se il mouse viene premuto all'interno dell'area
        // occupata da una macchina allora visualizza la Pagina
        // di editor della macchina
        //------------------------------------------------------
        if(yic > yfc){
           y_max=  yic;
           y_min=  yfc;
        }
        else{
           y_max=  yfc;
           y_min=  yic;
        }
        if(xic > xfc){
           x_max=  xic;
           x_min=  xfc;
        }
        else{
           x_max=  xfc;
           x_min=  xic;
        }
        if(( y_attivo > y_min )&&( y_attivo < y_max )){
           if(( x_attivo > x_min )&&( x_attivo < x_max )){
              Screen->Cursor=crHandPoint;
              sprintf(buffer,"TGV%02d", i);
              FormLayout->Hint=buffer;
              trovato=true;
              break;
           }
        }
     }
  }
  //-----------------------------------------------------
  // verifica se il mouse è all'interno di una stazione
  // **** CAMBIOBATTERIE ***
  //-----------------------------------------------------
  for(i=0; i<=TOTBATT; i++){
     //-------------------------------------------------------
     // Modifica coorinate nodi
     //-------------------------------------------------------
     if(ModificaCoordinateNodi==true) break;
     //-------------------------------------------------------
     // Abilitata la visualizzazione zone impianto --> skip!
     //-------------------------------------------------------
     if(VisualizzaZoneImpianto==true) break;
     //-------------------------------------------------------
     // Test percorsi Tecnoferrari --> skip!
     //-------------------------------------------------------
     if(TestPercorsiTecnoFerrari==true) break;
     //-------------------------------------------------------
     // Abilitata la Visualizzazione LENTE
     //-------------------------------------------------------
     //if(AbilitaLente==true && MoveOnLente==false) break;
     //-------------------------------------------------------
     // condizioni di skip
     //-------------------------------------------------------
     if(P.cb[i]->Num<1       ) continue;
     if(P.cb[i]->Num>TOTBATT ) continue;
     if(P.cb[i]->Punto==0    ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     num_nod = P.cb[i]->Punto;  // Numero del nodo
     //-------------------------------------------------------
     // Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offset[1][0])); // pixel x del nodo
     ycen = ((Punti[num_nod][1]-offset[1][1])); // pixel y del nodo
     //-------------------------------------------------------
     // Inizializzo offset di posizionamento
     //-------------------------------------------------------
     offsX = CBattGraph[i][0];  // offset manuale X
     offsY = CBattGraph[i][1];  // offset manuale Y
     //-----------------------------------------------
     // Punti dei vertici del poligono
     //-----------------------------------------------
     xinc1 = xcen - MezzoLatoX + offsX;    yinc1 = ycen - MezzoLatoY + offsY;
     xinc2 = xcen + MezzoLatoX + offsX;    yinc2 = ycen - MezzoLatoY + offsY;
     xinc3 = xcen + MezzoLatoX + offsX;    yinc3 = ycen + MezzoLatoY + offsY;
     xinc4 = xcen - MezzoLatoX + offsX;    yinc4 = ycen + MezzoLatoY + offsY;

     //------------------------------------------------------
     // Se il mouse viene mosso all'interno dell'area
     // del CAMBIOBATTERIA
     //------------------------------------------------------
     xic = xinc1/Rap*Rid_attivo;   yic = yinc1/Rap*Rid_attivo;
     xfc = xinc3/Rap*Rid_attivo;   yfc = yinc3/Rap*Rid_attivo;

     if(yic > yfc){ y_max = yic;   y_min = yfc; }
     else{          y_max = yfc;   y_min = yic; }

     if(xic > xfc){ x_max = xic;   x_min=  xfc; }
     else{          x_max = xfc;   x_min=  xic; }

     if(( y_attivo > y_min )&&( y_attivo < y_max )){
        if(( x_attivo > x_min )&&( x_attivo < x_max )){
           Screen->Cursor=crHandPoint;
           sprintf(buffer,"battery change %d",P.cb[i]->Num);
           FormLayout->Hint = buffer;
           trovato=true;
           break;
        }
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormLayout::FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  //--------------------------------------------------------
  // Blocco i controlli del mouse fino a che la pagina è
  // in apertura
  //--------------------------------------------------------
  if(FormMain->FormInOpen==true) return;
  //--------------------------------------------------------
  // Selezione area da zoommare
  //--------------------------------------------------------
  if(Button==mbRight){
     if(AbilitaZoom==true || ModificaCoordinateNodi==true){
        //---------------------------------------------
        // Se attiva selezione Zoom dinamica con mouse
        // allora recupera X2, Y2 per Zoom
        //---------------------------------------------
        if(ZoomSel==1){
           ZoomOk=1;      // indica che si può fare lo Zoom
           ZoomSel=0;
           ZoomX2=X;
           ZoomY2=Y;
        }
     }
  }
  //--------------------------------------------------------
  // Spostamento Layout
  //--------------------------------------------------------
  if(Button==mbLeft) SpostManAttivo = false;
}
//---------------------------------------------------------------------------
//-----------------------------------------------------
// Inizializzazione delle Variabili Globali
//-----------------------------------------------------
void TFormLayout::InizializzaVariabiliGrafiche()
{
  Lampeggio=true;
  //-----------------------------------
  // Test percorsi Tecnoferrari
  //-----------------------------------
  StartTestTecno  =0;
  EndTestTecno    =0;
  TestPercorsiTecnoFerrari = false;
  //-----------------------------------
  // Gestione ZOOM
  //-----------------------------------
  PosizionamentoSuNodo=0;
  //-----------------------------------
  // TGV LANCIATI IN RETTILINEO
  //-----------------------------------
  VisualizzaTgvLanciatiInRettilineo=false;
  //-----------------------------------
  // TGV TRASPARENTI
  //-----------------------------------
  memset(&VisualizzaTgvTrasparente[0], false, sizeof(VisualizzaTgvTrasparente));
  //-----------------------------------
  // Inizializza var. x Zoom con mouse
  //-----------------------------------
  AbilitaZoom=false;
  AbilitaSfondoGrafico=false;
  VisualizzaZoneMapping=false;
  VisualizzaZoneImpianto=false;
  ZoomSel=0;     // Zoom dinamico in corso
  ZoomX1=0;      // Zoom inizio X
  ZoomY1=0;      // Zoom inizio Y
  ZoomX2=0;      // Zoom fine X
  ZoomY2=0;      // Zoom fine Y
  ZoomOk=0;      // Indica che si può fare lo Zoom
//Rid   =RidAll; // Inizializzazione del rapporto di riduzione
}
//---------------------------------------------------------------------------
//------------------------------------------
// Zoom dinamico con Mouse
//------------------------------------------
void TFormLayout::Zoom_Dinamico_Mouse()
{
  int result;
  float NewRid;
  int origx, origy;
  int diffx, diffy;
  int appoggio1=0;
  int appoggio2=0;
  struct BLOCCOGRAFICO Dati;
  int i, j, x1, y1, x2, y2;
  short int dist, rot;
  short int sorg, dest;
  short int perc[MAXPERCORSI];
  struct DATI_PERCORSO dati[MAXPERCORSI];

  //------------------------------------------
  //
  //------------------------------------------
  if(AbilitaZoom==false && ModificaCoordinateNodi==false){
     //------- azzera variabili
     ZoomX1=ZoomX2=ZoomY1=ZoomY2=0;
     ZoomOk=0;
     return;
  }
  //------------------------------------------
  // Disegna riquadro selezione Zoom
  //------------------------------------------
  if( ZoomSel==1 ){
     if( ZoomX1!=ZoomX2 || ZoomY1!=ZoomY2 ){
        if(ModificaCoordinateNodi==false) Sfondo->Canvas->Pen->Color=clRed;
        else                              Sfondo->Canvas->Pen->Color=(TColor)(0x00FFFF80);
        Sfondo->Canvas->Brush->Color=clSFONDO;
        Sfondo->Canvas->Pen->Style=psDot;
        Sfondo->Canvas->MoveTo( ZoomX1, ZoomY1 );
        Sfondo->Canvas->LineTo( ZoomX1, ZoomY2 );
        Sfondo->Canvas->LineTo( ZoomX2, ZoomY2 );
        Sfondo->Canvas->LineTo( ZoomX2, ZoomY1 );
        Sfondo->Canvas->LineTo( ZoomX1, ZoomY1 );
        Sfondo->Canvas->Pen->Style=psSolid;
     }
  }

  //--------------------------------------------------
  // Se il Rapporto di riduzione è maggiore di quello
  // ammesso per l'Impianto allora --> Skip!
  //--------------------------------------------------
  if(Rid>MAXRID) return;
  if(ZoomSel==0 && ZoomOk==1){
     //--------------------------------------
     // GESTIONE DELLO ZOOM GRAFICO
     //--------------------------------------
     if(AbilitaZoom==true){
        if(!AbilitaSfondoGrafico){
           if( ZoomX1!=ZoomX2 || ZoomY1!=ZoomY2 ){
              //------------------------------------------
              // disegno    x = ( cm / RAP ) * Rid
              //            y = ( cm / RAP ) * Rid
              //------------------------------------------
              if(ZoomX2>=ZoomX1){
                 if(ZoomY2>=ZoomY1){
                    origx=(ZoomX1*Rap)/Rid;
                    origy=(ZoomY1*Rap)/Rid;
                    diffx=ZoomX2-ZoomX1;
                    diffy=ZoomY2-ZoomY1;
                 }
                 else{
                    origx=(ZoomX1*Rap)/Rid;
                    origy=(ZoomY2*Rap)/Rid;
                    diffx=ZoomX2-ZoomX1;
                    diffy=ZoomY1-ZoomY2;
                 }
              }
              else{
                 if(ZoomY2>=ZoomY1){
                    origx=(ZoomX2*Rap)/Rid;
                    origy=(ZoomY1*Rap)/Rid;
                    diffx=ZoomX1-ZoomX2;
                    diffy=ZoomY2-ZoomY1;
                 }
                 else{
                    origx=(ZoomX2*Rap)/Rid;
                    origy=(ZoomY2*Rap)/Rid;
                    diffx=ZoomX1-ZoomX2;
                    diffy=ZoomY1-ZoomY2;
                 }
              }
              //------------------------------------------
              // Calcolo OFFSETX e OFFSETY:
              // Lo spigolo in alto a sinistra dell'area
              // da zoommare deve coincidere con lo spigo-
              // lo in alto a sinistra dello schermo.
              //------------------------------------------
              appoggio1 = offset[1][0];
              appoggio2 = offset[1][1];
              appoggio1 = appoggio1 + origx;
              appoggio2 = appoggio2 + origy;
              //----------------------------------------
              // Calcolo rapporto di riduzione:
              // Per il calcolo del nuovo rapporto di
              // riduzione utilizzo il maggiore dei lati
              // dell'area selezionata.
              //----------------------------------------
              NewRid=Rid;
              if(diffx>=diffy){
                 NewRid = NewRid * SfondoW/(float)(diffx);
              }
              else{
                 NewRid = NewRid * SfondoH/(float)(diffy);
              }
              //----------------------------------------
              // Abilito lo Zoom dell'area selezionata
              // solo se il Nuovo rapporto di riduzione
              // non supera MAXRID.
              //----------------------------------------
              if(NewRid<MAXRID){
                 Rid=NewRid;
                 offset[1][0] = appoggio1;
                 offset[1][1] = appoggio2;
              }
           }
        }
        //--------------------------------------
        // GESTIONE DELLO SFONDO GRAFICO
        //--------------------------------------
        else{
           AbilitaZoom = false;
           memset(&Dati, 0, sizeof(BLOCCOGRAFICO));
           Dati.X = (int)(((ZoomX1*Rap)/Rid)+offset[1][0]);
           Dati.Y = (int)(((ZoomY1*Rap)/Rid)+offset[1][1]);
           Dati.LatoX = (int)(((ZoomX2*Rap)/Rid)+offset[1][0])-(((ZoomX1*Rap)/Rid)+offset[1][0]);
           Dati.LatoY = (int)(((ZoomY2*Rap)/Rid)+offset[1][0])-(((ZoomY1*Rap)/Rid)+offset[1][0]);
//riattiva SfondoGrafico
         //COMISfondo->DatiBlocco(&Dati);
        }
        //------- azzera variabili
        AbilitaZoom=false;
     }   
     //--------------------------------------
     // MODIFICA COORDINATE NODI
     //--------------------------------------
     if(ModificaCoordinateNodi==true){
        x1=y1=x2=y2=0;
        //--------------------------------------------------------
        // Normalizzazione delle coordinate dell'area selezionata
        //--------------------------------------------------------
        if(ZoomX2>=ZoomX1){
           if(ZoomY2>=ZoomY1){
              x1 = (int)(((ZoomX1*Rap)/Rid)+offset[1][0]);
              y1 = (int)(((ZoomY1*Rap)/Rid)+offset[1][1]);
              x2 = (int)(((ZoomX2*Rap)/Rid)+offset[1][0]);
              y2 = (int)(((ZoomY2*Rap)/Rid)+offset[1][1]);
           }
           else{
              x1 = (int)(((ZoomX1*Rap)/Rid)+offset[1][0]);
              y1 = (int)(((ZoomY2*Rap)/Rid)+offset[1][1]);
              x2 = (int)(((ZoomX2*Rap)/Rid)+offset[1][0]);
              y2 = (int)(((ZoomY1*Rap)/Rid)+offset[1][1]);
           }
        }
        else{
           if(ZoomY2>=ZoomY1){
              x1 = (int)(((ZoomX2*Rap)/Rid)+offset[1][0]);
              y1 = (int)(((ZoomY1*Rap)/Rid)+offset[1][1]);
              x2 = (int)(((ZoomX1*Rap)/Rid)+offset[1][0]);
              y2 = (int)(((ZoomY2*Rap)/Rid)+offset[1][1]);
           }
           else{
              x1 = (int)(((ZoomX2*Rap)/Rid)+offset[1][0]);
              y1 = (int)(((ZoomY2*Rap)/Rid)+offset[1][1]);
              x2 = (int)(((ZoomX1*Rap)/Rid)+offset[1][0]);
              y2 = (int)(((ZoomY1*Rap)/Rid)+offset[1][1]);
           }
        }
        //--------------------------------------------------------
        // Recupero tutti i nodi compresi nell'area selezionata
        //--------------------------------------------------------
        for(i=1; i<=MAXPUNTI; i++){
           if(Punti[ i ][2]==0) break;
           if(Punti[ i ][4]==0) continue;
           if(Punti[ i ][0]==0 && Punti[ i ][1]==0) continue;
           //------------------------------------------------------
           // Verifico se il Nodo è nel range selezionato
           //------------------------------------------------------
           if(( Punti[ i ][1]>=y1 )&&( Punti[ i ][1]<=y2 )){
              if(( Punti[ i ][0]>=x1 )&&( Punti[ i ][0]<=x2 )){
                 FormXYNodi->InsertNodi(i);
              }
           }
        }
        FormXYNodi->Refresh(FALSE);
     }
     //------- azzera variabili
     ZoomX1=ZoomX2=ZoomY1=ZoomY2=0;
     ZoomOk=0;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Aggiornamento Layout Sinottico
//---------------------------------------------------------------------------
void TFormLayout::AggiornaSinottico()
{
  int  i;
  bool modalita_box;
  int Xdifcm, Ydifcm;
  int Xatt, Yatt, Xdif, Ydif;

  static int   SpostLenteX=0;
  static int   SpostLenteY=0;
  static int   old_offx=offset[1][0];
  static int   old_offy=offset[1][1];
  static float old_Rid=RidAll;


  //----------------------------------------------------
  // Disegna sfondo nero Layout
  // In pratica cancella immagine precedente
  //----------------------------------------------------
  Sfondo->Canvas->Pen->Width   = 1;
  Sfondo->Canvas->Pen->Color   = clSFONDO;
  Sfondo->Canvas->Pen->Style   = psSolid;
  Sfondo->Canvas->Brush->Color = clSFONDO;
  Sfondo->Canvas->Brush->Style = bsSolid;
  Sfondo->Canvas->Rectangle(0, 0, SfondoW, SfondoH);
  //----------------------------------------------------
  // Re inizializzazione ZOOM LENTE
  //----------------------------------------------------
  Sfondo1->Canvas->Pen->Width   = 1;
  Sfondo1->Canvas->Pen->Color   = clSFONDO;
  Sfondo1->Canvas->Pen->Style   = psSolid;
  Sfondo1->Canvas->Brush->Color = clSFONDO;
  Sfondo1->Canvas->Brush->Style = bsSolid;
  Sfondo1->Canvas->Rectangle(0, 0, SfondoW, SfondoH);
  //----------------------------------------------------
  // Visualizzazione dello sfondo grafico
  //----------------------------------------------------
  if( Rid>=RID_SFONDO ) SfondoGrafico();
  //----------------------------------------------------
  // Disegno i numeri dei nodi
  //----------------------------------------------------
  if( Rid>=RID_NUMERI ) disegna_numeri();
  //----------------------------------------------------
  // Disegno dei percorsi dell'impianto
  //----------------------------------------------------
  disegna_percorso();           // -- percorsi statici
  disegna_percorso_tgv();       // -- percorsi dinamici
  //----------------------------------------------------
  // Gestione TEST PERCORSI TECNOFERRARI
  //----------------------------------------------------
  if(TestPercorsiTecnoFerrari==true) disegna_test_percorsi();
  //----------------------------------------------------
  // Disegno delle strutture (TGV, MACCHINE, MAGAZZINI)
  //----------------------------------------------------
  for( i=1; i<=MAXAGV; i++ ){
     //------------------------------------------------------
     // Gestione TGV FORK
     //------------------------------------------------------
     modalita_box = (bool)(TestBit((char *)&ABILITATGVBOX, i-1));
     //------------------------------------
     // Visualizzazione TGV
     //------------------------------------
     if(modalita_box==false) disegna_agv(i);
     else                    disegna_agv_box(i);
     DisegnaRiflettoriLaser(i);
  }
  //----------------------------------------------------
  // DISEGNO STRUTTURE IMPIANTO
  //----------------------------------------------------
  disegna_porte();
  disegna_magazzini();
  disegna_macchine();
  disegna_baie();
  disegna_cbatt();

  //----------------------------------------------------
  // Disegni specializzati per gli impianti
  //----------------------------------------------------
  disegna_speciali_IMPIANTI();

  //----------------------------------------------------
  // Visualizzazione richiesta da POSIZIONAMENTO SU NODO
  //----------------------------------------------------
  if(PosizionamentoSuNodo!=0) disegna_numero_richiesto();

  //----------------------------------------------------
  // Zoom dell'Impianto tramite mouse
  //----------------------------------------------------
  Zoom_Dinamico_Mouse();

  //----------------------------------------------------
  // copia lo sfondo su FormLayout
  //----------------------------------------------------
  Canvas->Draw(0, 1, FormLayout->Sfondo);    // Layout
  old_Rid       = Rid;
  old_offx      = offset[1][0];
  old_offy      = offset[1][1];
  offset1[1][0] = offset[1][0];
  offset1[1][1] = offset[1][1];
  SpostLenteX   = 0;
  SpostLenteY   = 0;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_numeri()
// ----------------------------------------------
// evitare di disegnare il numero se appartiene a macchina o magazzino
//
void TFormLayout::disegna_numeri()
/*******************/
{
  int grd, i, j;
  int X1, Y1, X2, Y2;
  TColor col;
  char num[5];
  bool nodo_fuori_ingombro;
  char all_mess[101]="";

  for(i=1;i<=TOTPUNTI;i++ ){
     //-------------------------------------------------------------------
     // Nessuna pagina di appartenenza --> Skip!!
     //-------------------------------------------------------------------
     if(Punti[i][4]==0) continue;
     //-------------------------------------------------------------------
     // Nodo impostato come punto terminale --> Skip!!
     //-------------------------------------------------------------------
     if(i>50 && !VisualizzaZoneImpianto && N.punti_notevoli[Punti[i][2]]) continue;
     //-------------------------------------------------------------------
     // assegnazione del colore al nodo in base alla zona di appartenenza
     //-------------------------------------------------------------------
     col = clNODI;
     if(VisualizzaZoneImpianto && N.n[Punti[i][2]]->num){
        if(N.n[Punti[i][2]]->zp==1) col = clWhite;
        if(N.n[Punti[i][2]]->zp==2) col = clAqua;
        if(N.n[Punti[i][2]]->zp==3) col = clLime;
        if(N.n[Punti[i][2]]->zp==4) col = clRed;
        if(N.n[Punti[i][2]]->zp==5) col = clOlive;
        if(N.n[Punti[i][2]]->zp==6) col = clFuchsia;
        if(N.n[Punti[i][2]]->zp==7) col = clTeal;
        if(N.n[Punti[i][2]]->zp==8) col = clPurple;
        if(N.n[Punti[i][2]]->zp==9) col = clMaroon;
     }
     X1 = (int)((Punti[i][0]-offset[Punti[ i ][4]][0])/Rap*Rid)-1;
     Y1 = (int)((Punti[i][1]-offset[Punti[ i ][4]][1])/Rap*Rid)-1;
     X2 = (int)((Punti[i][0]-offset[Punti[ i ][4]][0])/Rap*Rid)+3;
     Y2 = (int)((Punti[i][1]-offset[Punti[ i ][4]][1])/Rap*Rid)+3;
     //-------------------------------------------------------------------
     // Visualizza punti FUORI INGOMBRO
     // Verifico se il nodo è un punto fuori ingombro e gli assegno
     // un colore differente
     //-------------------------------------------------------------------
     if(!VisualizzaZoneImpianto && N.n[Punti[i][2]]->num){
        nodo_fuori_ingombro=false;
        for(j=0; j<MAXNODIATTESA; j++){
           if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1                     ) continue;
           if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>MAXPUNTI              ) continue;
           if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=N.n[Punti[i][2]]->num) continue;
           nodo_fuori_ingombro=true;
           break;
        }
        if(nodo_fuori_ingombro==true) col = clNODI_FING;
     }

     //-------------------------------------------------------------------
     // Se il Punto è di RIENTRO in traccia viene disegnato in arancio.
     //-------------------------------------------------------------------
     if(N.n[Punti[i][2]]->typ==TYP_NODO_RIENTRO) col = (TColor)(0x000080FF);

     itoa(Punti[i][2], num, 10);
     StampNum(FormLayout->Sfondo, X1, Y1, num , col, FALSE, Punti[i][3]);
     //-------------------------------------------------------------------
     // VISUALIZZAZIONE GRADI INCLINAZIONE IN DEVIAZIONE
     // se il nodo è occupato da un TGV visualizzo anche i gradi
     // assegnati al nodo stesso
     //-------------------------------------------------------------------
     if(N.nodo_busy[i]>0 && N.nodo_busy[i]<=MAXAGV && VisualizzaZoneMapping==false){
        grd = N.nodo_grd[N.nodo_busy[i]][i];
        if(grd!=0 && grd!=90 && grd!=270 && grd!=360){
           itoa(grd, num, 10);
           StampNum(FormLayout->Sfondo, X1, Y1+10, num , AGV[N.nodo_busy[i]]->mission.Col, FALSE, 0);
        }
     }
     //-------------------------------------------------------------------
     // Visualizzazione ZONE MAPPING
     //-------------------------------------------------------------------
     if(VisualizzaZoneMapping==true){
        itoa(N.n[i]->ZoneMap, num, 10);
        StampNum(FormLayout->Sfondo, X1, Y1+10, num , clWhite, FALSE, 0);
     }
     //-------------------------------------------------------------------
     // GESTIONE PRECEDENZE PER TGV IN RETTILINEO
     //-------------------------------------------------------------------
     if(VisualizzaTgvLanciatiInRettilineo==true && N.nodo_con_tgv_lanciato[i]>0 && N.nodo_con_tgv_lanciato[i]<=MAXAGV){
        col = AGV[N.nodo_con_tgv_lanciato[i]]->mission.Col;
     }
     //-------------------------------------------------------------------
     // Visualizzazione del numero sul sinottico dell'impianto
     //-------------------------------------------------------------------
     col = clRed;
     Sfondo->Canvas->Pen->Color   = col;
     Sfondo->Canvas->Brush->Color = col;
     Sfondo->Canvas->Ellipse( X1, Y1, X2, Y2);
     //-------------------------------------------------------------------
     // Visualizzazione ingombro grafico sul punto
     //-------------------------------------------------------------------
     for(j=1;j<=MAXAGV;j++){
        if(ModificaCoordinateNodi==true){
           if(FormXYNodi->PuntoPerIngombro[j]==i){
              IngombroAgvGrafico[j]->SettaIngombroSuTratto(i, i, FormXYNodi->RotazioneIngombroS[j], all_mess);
              IngombroAgvGrafico[j]->disegna_ingombro_agv(&IngombroAgvGrafico[1], MAXAGV-1, Sfondo);
           }
        }
        else{
           if(FormXYNodi->PuntoPerIngombro[j]==i){
              IngombroAgvGrafico[j]->SettaIngombroSuTratto(i, i, FormXYNodi->RotazioneIngombroS[j], all_mess);
              IngombroAgvGrafico[j]->disegna_ingombro_agv(&IngombroAgvGrafico[1], MAXAGV-1, Sfondo);
           }
        }
     }
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_numero_richiesto()
// ----------------------------------------------
// Disegna il numero richiesto dal posizionamento su nodo
//
void TFormLayout::disegna_numero_richiesto()
/*******************/
{
  TColor col = clYellow;;
  char num[5];
  int i, x_num, y_num;

  for(i=1;i<=TOTPUNTI;i++ ){
     //-------------------------------------------------------------------
     // Nessuna pagina di appartenenza --> Skip!!
     //-------------------------------------------------------------------
     if(Punti[i][4]==0          ) continue;
     //-------------------------------------------------------------------
     // Nodo non selezionato
     //-------------------------------------------------------------------
     if(i!=PosizionamentoSuNodo ) continue;
     //-------------------------------------------------------------------
     // Recupero le coordinate dei punti
     //-------------------------------------------------------------------
     x_num = (int)((Punti[i][0]-offset[Punti[ i ][4]][0])/Rap*Rid)-1;
     y_num = (int)((Punti[i][1]-offset[Punti[ i ][4]][1])/Rap*Rid)-1;
     //-------------------------------------------------------------------
     // Evidenzio il nodo selezionato
     //-------------------------------------------------------------------
     Sfondo->Canvas->Pen->Width   = 2;
     Sfondo->Canvas->Pen->Color   = clYellow;
     Sfondo->Canvas->Brush->Color = clYellow;
     Sfondo->Canvas->Brush->Style = bsClear;
     Sfondo->Canvas->Ellipse( x_num-20, y_num-20, x_num+20, y_num+20 );
     Sfondo->Canvas->Pen->Width   = 1;
     //-------------------------------------------------------------------
     // Visualizzazione del numero sul sinottico dell'impianto
     //-------------------------------------------------------------------
     itoa(Punti[i][2], num, 10);
     StampNum(FormLayout->Sfondo, x_num, y_num, num, col, FALSE, 0);
     break;
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_percorso()
// ----------------------------------------------
// Disegna percorso statico
//
void TFormLayout::disegna_percorso()
/*********************************/
{
  char all_mess[101]="";
  TColor col=clLINEE;
  int i, j, prec, succ;
  int Grado, Quadrante, Raggio;
  int tot_punti;
  TPoint points[5];
  bool DirezioneAgvGrafico;

  prec=succ=0;
  Sfondo->Canvas->Pen->Style=psSolid;
  //---------------------------------------------------------
  // DISEGNA PERCORSI STATICI
  //---------------------------------------------------------
  for(i=0;;i++){
     prec = Percorso[i][0];
     succ = Percorso[i][1];
     //-----------------------
     // fine percorso -> esci
     //-----------------------
     if(prec==0 && succ==0) break;
     //----------------------------------------------------
     // Non disegna i punti a cui non è stata assegnata una
     // Pagina Grafica.
     //----------------------------------------------------
     if(Punti[prec][4]==0 || Punti[succ][4]==0) continue;
     //-----------------------------------------------------------
     // Solo se i due punti fanno parte della pagina grafica
     // "pagina" allora traccia linea
     //-----------------------------------------------------------
     linea_punti(prec, succ, col);
     //-----------------------------------------------------------------------------------------
     // In modifica coordinate nodi (AlgoCad) disegno anche l'ingombro degli Agv (se impostato)
     //-----------------------------------------------------------------------------------------
     for(j=1;j<=MAXAGV;j++){
        if(ModificaCoordinateNodi==true){
           if(FormXYNodi->SegmentoPerIngombro[j]==i){
              DirezioneAgvGrafico=FormXYNodi->DirezionePerIngombro[j];
              IngombroAgvGrafico[j]->SettaIngombroSuTratto(prec, succ, DirezioneAgvGrafico, all_mess);
              IngombroAgvGrafico[j]->disegna_ingombro_agv(&IngombroAgvGrafico[1], MAXAGV-1, Sfondo);
           }
        }
        else{
           if(FormXYNodi->SegmentoPerIngombro[j]==i){
              DirezioneAgvGrafico=FormXYNodi->DirezionePerIngombro[j];
              IngombroAgvGrafico[j]->SettaIngombroSuTratto(prec, succ, DirezioneAgvGrafico, all_mess);
              IngombroAgvGrafico[j]->disegna_ingombro_agv(&IngombroAgvGrafico[1], MAXAGV-1, Sfondo);
           }
        }
     }
  }
  //---------------------------------------------------------
  // DISEGNA PERCORSI CURVE STATICI
  //---------------------------------------------------------
  for(i=0;;i++){
     //----------------------------------------------------
     // Curve di Bezier attive --> skip!
     //----------------------------------------------------
     if(CurveDiBezier!=0) break;
     //----------------------------------------------------
     // Parametri curva
     //----------------------------------------------------
     prec      = PercorsoCurva[i][0];
     succ      = PercorsoCurva[i][1];
     Quadrante = PercorsoCurva[i][2];
     Raggio    = PercorsoCurva[i][3];
     //----------------------------------------------------
     // fine percorso -> esci
     //----------------------------------------------------
     if(prec==0 && succ==0) break;
     //----------------------------------------------------
     // Non disegna i punti a cui non è stata assegnata una
     // Pagina Grafica.
     // Se tipo di visualizzazione ==2 NON VISUALIZZARE
     // nei percorsi statici ma solo durante missione
     //----------------------------------------------------
     if( Punti[prec][4]==0 || Punti[succ][4]==0) continue;
     if( PercorsoCurva[i][4] ==0 ) continue;
     if( PercorsoCurva[i][4] ==2 ) continue;
     //-----------------------------------------------------------
     // Solo se i due punti fanno parte della pagina grafica
     // "pagina" allora traccia linea
     //-----------------------------------------------------------
     curva_punti( prec, succ, Quadrante, Raggio,  col );
  }
  //---------------------------------------------------------
  // DISEGNA PERCORSI CURVE BEZIER
  //---------------------------------------------------------
  for(i=0;;i++){
     //-----------------------------------------------
     // Curve di Bezier disattivate --> skip!
     //-----------------------------------------------
     if(CurveDiBezier==0) break;
     //-----------------------------------------------
     // Parametri curve
     //-----------------------------------------------
     tot_punti = 0;
     Grado     = PercorsoCurvaBezier[i][0];
     prec      = PercorsoCurvaBezier[i][1];
     succ      = PercorsoCurvaBezier[i][2];
     //-----------------------------------------------
     // Fine struttura -> skip
     //-----------------------------------------------
     if(prec==0  && succ==0) break;
     //-----------------------------------------------
     // Tipo di curve in visualizzazione
     // CurveDiBezier==2 --> SOLO QUADRATICE
     // CurveDiBezier==3 --> SOLO CUBICHE
     //-----------------------------------------------
     if(CurveDiBezier==2 && Grado!=2) continue;
     if(CurveDiBezier==3 && Grado!=3) continue;
     //-----------------------------------------------
     // Curva esistente, ma disattivata
     //-----------------------------------------------
     if(Grado==0 && (prec!=0 || succ!=0)) continue;
     //-----------------------------------------------
     // Compilazione array di punti nell'ordine
     // di visualizzazione in base al grado della
     // curva
     //
     // (1) QUADRATICA - GRADO 2
     //     Pstart + Pcontrol1 + PEnd
     //
     // (2) CUBICA     - GRADO 3
     //     Pstart + Pcontrol1 + Pcontrol2 + PEnd
     //
     //-----------------------------------------------
     // (1)
     if(Grado==2){
        tot_punti=3;
        //-----------------------------------------------------------------
        // Coordinate ingresso e uscita recuperate da struttura Punti[][]
        //-----------------------------------------------------------------
        points[0] = Point(((Punti[prec  ][0]-offset[1][0])/Rap)*Rid, ((Punti[prec  ][1]-offset[1][1])/Rap)*Rid);
        points[2] = Point(((Punti[succ  ][0]-offset[1][0])/Rap)*Rid, ((Punti[succ  ][1]-offset[1][1])/Rap)*Rid);
        //-----------------------------------------------------------------
        // Coordinate vertici di controllo espresse direttamente in X, Y
        //-----------------------------------------------------------------
        points[1] = Point(((PercorsoCurvaBezier[i][3]-offset[1][0])/Rap)*Rid, ((PercorsoCurvaBezier[i][4]-offset[1][1])/Rap)*Rid);

        //
        // tmp forzo colore rosso per differenziare da altre curve (solo se CurveDiBezier==1)
        //
        if(CurveDiBezier==1) col = clRed;
     }
     // (2)
     if(Grado==3){
        tot_punti=4;
        //-----------------------------------------------------------------
        // Coordinate ingresso e uscita recuperate da struttura Punti[][]
        //-----------------------------------------------------------------
        points[0] = Point(((Punti[prec  ][0]-offset[1][0])/Rap)*Rid, ((Punti[prec  ][1]-offset[1][1])/Rap)*Rid);
        points[3] = Point(((Punti[succ  ][0]-offset[1][0])/Rap)*Rid, ((Punti[succ  ][1]-offset[1][1])/Rap)*Rid);
        //-----------------------------------------------------------------
        // Coordinate vertici di controllo espresse direttamente in X, Y
        //-----------------------------------------------------------------
        points[1] = Point(((PercorsoCurvaBezier[i][3]-offset[1][0])/Rap)*Rid, ((PercorsoCurvaBezier[i][4]-offset[1][1])/Rap)*Rid);
        points[2] = Point(((PercorsoCurvaBezier[i][5]-offset[1][0])/Rap)*Rid, ((PercorsoCurvaBezier[i][6]-offset[1][1])/Rap)*Rid);
        //
        // tmp forzo colore verde per differenziare da altre curve (solo se CurveDiBezier==1)
        //
        if(CurveDiBezier==1) col = clLime;
     }
     //-----------------------------------------------
     // Nessuna curva impostata
     // Gradi diversi da
     // "2 = QUADRATICA"
     // "3 = CUBICA"
     // non sono ammessi e vengono considerati
     // come curva disattivata
     //-----------------------------------------------
     if(tot_punti==0) continue;
     //-----------------------------------------------
     // 4 PUNTI DI CONTROLLO = CURVA CUBICA
     // 3 PUNTI DI CONTROLLO = CURVA QUADRATICA
     //-----------------------------------------------
   //Sfondo->Canvas->PolyBezier(points, tot_punti-1);
     if(ModificaCoordinateNodi==true && FormXYNodi->CurvaSelezionata==i){modifica_curva_punti_bezier( points, tot_punti-1);}
     else curva_punti_bezier( points, tot_punti-1, col);
     //-----------------------------------------------------------------------------------------
     // In modifica coordinate nodi (AlgoCad) disegno anche l'ingombro degli Agv (se impostato)
     //-----------------------------------------------------------------------------------------
     for(j=1;j<=MAXAGV;j++){
        if(ModificaCoordinateNodi==true){
           if(FormXYNodi->CurvaPerIngombro[j]==i){
              DirezioneAgvGrafico=FormXYNodi->DirezionePerIngombro[j];
              IngombroAgvGrafico[j]->SettaIngombroSuTratto(prec, succ, DirezioneAgvGrafico, all_mess);
              IngombroAgvGrafico[j]->disegna_ingombro_agv(&IngombroAgvGrafico[1], MAXAGV, Sfondo);
           }
        }
        else{
           if(FormXYNodi->CurvaPerIngombro[j]==i){
              DirezioneAgvGrafico=FormXYNodi->DirezionePerIngombro[j];
              IngombroAgvGrafico[j]->SettaIngombroSuTratto(prec, succ, DirezioneAgvGrafico, all_mess);
              IngombroAgvGrafico[j]->disegna_ingombro_agv(&IngombroAgvGrafico[1], MAXAGV, Sfondo);
           }
        }
     }
  }
}
//---------------------------------------------------------------------------

// ----------------------------------------------
//       disegna_percorso_tgv()
// ----------------------------------------------
// Disegna percorso Occupato o Prenotato dal TGV
//
void TFormLayout::disegna_percorso_tgv()
/*********************************/
{
  TColor col=clLINEE;
  short int start;
  int i, j, k, prec, succ, typ_curva;
  bool alternativo;
  bool TrattoOccupato;
  int  quadrante, raggio;
  TPoint points[5];

  col=clLINEE;
  Sfondo->Canvas->Pen->Style=psSolid;
  //---------------------------------------------------------
  // DISEGNA PERCORSI DINAMICI
  //---------------------------------------------------------
  for(i=1; i<=MAXAGV; i++){
     //-----------------------------------------------------
     // Individuo l'indice della posizione del TGV nella
     // missione.
     //-----------------------------------------------------
     start=0;
     for(j=0; j<MAXPERCORSI; j++){
        if(AGV[i]->mission.punto[j]==0                  ) break;
        if(AGV[i]->mission.punto[j]!=AGV[i]->stato.pos  ) continue;
        start=(short int)(j);
        break;
     }
     //-----------------------------------------------------
     // Scorrimento della struttura della missione del TGV
     //-----------------------------------------------------
     for(j=start; j<MAXPERCORSI; j++){
        TrattoOccupato=false;
        if(AGV[i]->mission.punto[j+1]==0) break;
        prec  = AGV[i]->mission.punto[j];
        succ  = AGV[i]->mission.punto[j+1];
        //-----------------------------------------------------
        // Se il tratto di percorso in questione è già occupato
        // da un'altra navetta --> continue.
        //-----------------------------------------------------
        if(N.nodo_busy[prec]!=0 && N.nodo_busy[prec]!=i) continue;
        if(N.nodo_busy[succ]!=0 && N.nodo_busy[succ]!=i) continue;
        //-----------------------------------------------------
        // Scelta del colore della traccia in base al numero
        // del TGV.
        //
        // 17/06/2011
        // Visualizzo i percorsi colorati TGV solo con
        // missione in corso o TGV in manuale per vedere
        // i nodi che sono restati occupati dal reset_manuale
        //-----------------------------------------------------
        //col = AGV[i]->mission.Col;
        if(!AGV[i]->stato.s.bit.funz || (AGV[i]->stato.s.bit.funz && AGV[i]->stato.start!=0)){
           col = AGV[i]->mission.Col;
        }
        else{
           col = clLINEE;
        }
        //-----------------------------------------------------
        // ROTAZIONE
        //  0... 3 - RETTILINEI
        //  4...19 - ARCHI DI CIRCONFERENZA
        // 20...23 - CURVE DI BEZIER QUADRATICHE
        // 30...33 - CURVE DI BEZIER CUBICHE
        // ....... - free
        // 50...70 - DEVIAZIONI RETTILINEE
        //-----------------------------------------------------
        typ_curva = 0;
        if(AGV[i]->mission.dati_perc[j].rot>3  && AGV[i]->mission.dati_perc[j].rot<20) typ_curva = 1; // ARCHI DI CRF
        if(AGV[i]->mission.dati_perc[j].rot>19 && AGV[i]->mission.dati_perc[j].rot<30) typ_curva = 2; // BEZIER QUADRATICA
        if(AGV[i]->mission.dati_perc[j].rot>29 && AGV[i]->mission.dati_perc[j].rot<40) typ_curva = 3; // BEZIER CUBICA
        //-----------------------------------------------------
        // ARCHI DI CIRVONFERENZA
        // Le rotazioni da 4 a 19 sono utilizzate per la
        // gestione degli archi di circonferenza
        //-----------------------------------------------------
        if(typ_curva==1){
           raggio = quadrante = 0;
           //---------------------------------------------------------
           // ARCHI DI CIRVONFERENZA
           // Verifico se il tratto appartiene alla stuttura curve
           //---------------------------------------------------------
           for(k=0;;k++){
              //---------------------------------------------------------
              // Fine struttura --> skip!
              //---------------------------------------------------------
              if(PercorsoCurva[k][0]==0 && PercorsoCurva[k][1]==0 && PercorsoCurva[k][3]==0) break;
              //---------------------------------------------------------
              // Recupero il quadrante di appartenenza della curva
              //---------------------------------------------------------
              if((prec==PercorsoCurva[k][0] && succ==PercorsoCurva[k][1]) || (prec==PercorsoCurva[k][1] && succ==PercorsoCurva[k][0])){
                 quadrante = PercorsoCurva[k][2];
                 raggio    = PercorsoCurva[k][3];
                 break;
              }
           }
        }
        //-----------------------------------------------------
        // CURVE DI BEZIER
        // Le rotazioni da 20 a 39 sono utilizzate per la
        // gestione degli archi di circonferenza
        //-----------------------------------------------------
        if(typ_curva==2 || typ_curva==3){
           N.CercaCurvaBezier(TRUE, prec, succ, &typ_curva, &points[0]);
        }
        //-----------------------------------------------------------
        // Se numero percorsi è PARI allora commuta il flag
        //-----------------------------------------------------------
        alternativo= false;
        if( Lampeggio  && ((j-1) != (j & 0xfffe))) alternativo= true;
        if( !Lampeggio && ((j-1) == (j & 0xfffe))) alternativo= true;
        //-----------------------------------------------------------
        // Se il percorso è occupato faccio una linea continua.
        //-----------------------------------------------------------
        if(N.nodo_busy[prec]==i && N.nodo_busy[succ]==i) TrattoOccupato=true;
        //-----------------------------------------------------------
        // Se il percorso è solo prenotato gestisco il lampeggio.
        //-----------------------------------------------------------
        if(!TrattoOccupato && !alternativo             ) col = clLINEE;
        //-----------------------------------------------------------
        // Solo se i due punti fanno parte della pagina grafica
        // "pagina" allora traccia linea
        //-----------------------------------------------------------
        if(typ_curva==0) linea_punti(prec, succ, col);                       // RETTILINEO
        if(typ_curva==1) curva_punti(prec, succ, quadrante, raggio, col);    // ARCHI CRF
        if(typ_curva==2) curva_punti_bezier(points, 2, col);                 // BEZIER QUADRATICA
        if(typ_curva==3) curva_punti_bezier(points, 3, col);                 // BEZIER CUBICA
     }
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_test_percorsi()
// ----------------------------------------------
// Restitusce un'array di punti del percorso selezionato dal
// tecnico TecnoFerrari per il TEST PERCORSI TECNOFERRARI.
//
void TFormLayout::disegna_test_percorsi()
/*********************************/
{
  int i, k, err;
  int typ_curva;
  TColor col=clLINEE;
  short int rot, dist;
  short int prec, succ, quadrante;
  char buffer[101]="";
  char all_mess[101]="";
  short int perc[MAXPERCORSI];
  short int app_perc[MAXPERCORSI];
  short int old_perc[MAXPERCORSI];
  struct DATI_PERCORSO dati[MAXPERCORSI];
  struct DATI_PERCORSO old_dati[MAXPERCORSI];
  static short int tipo_path=FAST_PATH;
  TPoint points[5];

  err=0;
  col=clLINEE;
  Sfondo->Canvas->Pen->Style=psSolid;
  FormMain->SBarImpianto->Panels->Items[0]->Text = "";
  //-------------------------------------------------------------
  // Se mancano le informazioni di percorso --> Skip!
  //-------------------------------------------------------------
  if(EndTestTecno==0               ) return;
  if(StartTestTecno==0             ) return;
  if(EndTestTecno==StartTestTecno  ) return;
  //-------------------------------------------------------------
  // Inizializzazione variabili.
  //-------------------------------------------------------------
  rot=dist=0;
  memset(&perc[0],     0, sizeof(perc));
  memset(&app_perc[0], 0, sizeof(app_perc));
  memset(&old_perc[0], 0, sizeof(old_perc));
  memset(&dati[0],     0, sizeof(dati));
  memset(&old_dati[0], 0, sizeof(old_dati));
  //-------------------------------------------------------------
  // Abilita visualizzazione percorsi STANDARD o VELOCI
  //-------------------------------------------------------------
  if(AbilitaGestionePercorsiVeloci==true){
     if(tipo_path==FAST_PATH) tipo_path=STD_PATH;
     else                     tipo_path=FAST_PATH;
  }
  else{
     tipo_path=STD_PATH;
  }
  //-------------------------------------------------------------
  // PERCORSO DIRETTO
  // Verifica il percorso originale del TGV da START a DEST.
  //-------------------------------------------------------------
  err = AGV[1]->distanza(tipo_path, StartTestTecno, EndTestTecno, old_perc, old_dati, buffer, &dist, &rot);
  if(err!=0 || dist>MAXPERCORSI){
     sprintf(all_mess, "DS - %s", buffer);
     FormMain->SBarImpianto->Panels->Items[0]->Text = all_mess;
   //EndTestTecno=StartTestTecno=0;
     return;
  }
  //-------------------------------------------------------------
  // Verifica se esiste la GESTIONE DELLE CURVE
  //-------------------------------------------------------------
  if(AGV[1]->mission.GestioneLaser==true){
     err = P.ImpostazioneCurve(old_perc, old_dati, perc, dati, buffer);
     if(err!=0){
        sprintf(all_mess, "CR - %s", buffer);
        FormMain->SBarImpianto->Panels->Items[0]->Text = all_mess;
      //EndTestTecno=StartTestTecno=0;
        return;
     }
     //-------------------------------------------------------------------
     // Modifico il percorso in caso di curve
     //-------------------------------------------------------------------
     memcpy(&old_perc, &perc, sizeof(old_perc));
     memcpy(&old_dati, &dati, sizeof(old_dati));
  }
  //-------------------------------------------------------------
  // Verifica se esiste la GESTIONE DELLE DEVIAZIONI
  //-------------------------------------------------------------
  err = P.ImpostazioneDeviazioni(1, old_perc, old_dati, perc, dati, buffer);
  if(err!=0){
     sprintf(all_mess, "DV - %s", buffer);
     FormMain->SBarImpianto->Panels->Items[0]->Text = all_mess;
   //EndTestTecno=StartTestTecno=0;
     return;
  }
  //-------------------------------------------------------------
  // Rappresentazione grafica del percorso testato
  //-------------------------------------------------------------
  for(i=0; i<MAXPERCORSI; i++){
     typ_curva = 0;
     quadrante = 0;
     prec      = perc[i];
     succ      = perc[i+1];
     if(succ==0) break;
     //-----------------------------------------------------
     // ROTAZIONE
     //  0... 3 - RETTILINEI
     //  4...19 - ARCHI DI CIRCONFERENZA
     // 20...23 - CURVE DI BEZIER QUADRATICHE
     // 30...33 - CURVE DI BEZIER CUBICHE
     // ....... - free
     // 50...70 - DEVIAZIONI RETTILINEE
     //-----------------------------------------------------
     typ_curva = 0;
     if(dati[i].rot>3  && dati[i].rot<20) typ_curva = 1; // ARCHI DI CRF
     if(dati[i].rot>19 && dati[i].rot<30) typ_curva = 2; // BEZIER QUADRATICA
     if(dati[i].rot>29 && dati[i].rot<40) typ_curva = 3; // BEZIER CUBICA
     //-----------------------------------------------------
     // ARCHI DI CIRCONFERENZA
     // Recupero il quadrante nel caso di archi di CRF
     //-----------------------------------------------------
     if(typ_curva==1){
        switch(dati[i].rot){
           case  4: quadrante = 3;
                    break;
           case  5: quadrante = 1;
                    break;
           case  6: quadrante = 4;
                    break;
           case  7: quadrante = 2;
                    break;
           case  8: quadrante = 2;
                    break;
           case  9: quadrante = 4;
                    break;
           case 10: quadrante = 1;
                    break;
           case 11: quadrante = 3;
                    break;
           case 12: quadrante = 3;
                    break;
           case 13: quadrante = 1;
                    break;
           case 14: quadrante = 4;
                    break;
           case 15: quadrante = 2;
                    break;
           case 16: quadrante = 2;
                    break;
           case 17: quadrante = 4;
                    break;
           case 18: quadrante = 1;
                    break;
           case 19: quadrante = 3;
                    break;
           default: quadrante = 0;
                    break;
        }
     }
     //-----------------------------------------------------
     // CURVE DI BEZIER
     // Recupero i vertici di controllo
     //-----------------------------------------------------
     if(typ_curva==2 || typ_curva==3){
        //---------------------------------------------------------
        // CURVE DI BEZIER
        // Verifico se il tratto appartiene alla stuttura curve
        //---------------------------------------------------------
        N.CercaCurvaBezier(TRUE, prec, succ, &typ_curva, &points[0]);
     }
     //-----------------------------------------------------------
     // Assegno colore in base al tipo di percorso selezionato
     //-----------------------------------------------------------
     col = clLime;
     if(tipo_path==FAST_PATH) col=clRed;
     //-----------------------------------------------------------
     // Solo se i due punti fanno parte della pagina grafica
     // "pagina" allora traccia linea
     //-----------------------------------------------------------
     if(typ_curva==0) linea_punti(prec, succ, col);               // RETTILINEO
     if(typ_curva==1) curva_punti(prec, succ, quadrante, 0, col); // ARCHI CRF
     if(typ_curva==2) curva_punti_bezier(points, 2, col);         // BEZIER QUADRATICA
     if(typ_curva==3) curva_punti_bezier(points, 3, col);         // BEZIER CUBICA
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_agv()
// ----------------------------------------------
// Num = numero AGV (1..MAXAGV)
// Entro nella funzione dopo aver fatto un LOAD_STATO_AGV.
//
//  2) AGV[Num]->stato.offset_da_nodo = distanza da nodo in Impulsi
//     1 Impulso = 4 cm,    spostamento in pixel = (Offset*4)/RAP
//
//  3) Spostamento da nodo: 0ø + Avanti     = X - spostamento
//                          0ø + Indietro   = X + spostamento
//                         90ø + Avanti     = Y - spostamento
//                         90ø + Indietro   = Y + spostamento
//                         AGV su nodo -> spostamento = 0
//
void TFormLayout::disegna_agv( int Num )
/***************************************/
{
  int i, j;
  int lung_agv, larg_agv;
  int lung_all, larg_all;
  int lung_cest, larg_cest;
  int xcen,ycen,xlog,ylog,xpal,ypal,xx1,xx2,yy1,yy2;
  int xs1,xs2,ys1,ys2,xs3,ys3,xs4,ys4;
  int Xl1,Xl2,Yl1,Yl2,Xl3,Yl3,Xl4,Yl4;
  int xlog1,xlog2,ylog1,ylog2,xlog3,ylog3,xlog4,ylog4;
  int xtgv1,xtgv2,ytgv1,ytgv2,xtgv3,ytgv3,xtgv4,ytgv4;
  int xb11, xb12, yb11, yb12, xb13, yb13, xb14, yb14;
  int xbr1, xbr2, ybr1, ybr2;
  int nod, su_nod, pixel_da_nodo, avanti, indietro, carA, carB;
  int mezzo_lung, mezzo_larg, offs, offs1;
  int offsetX_plog, offsetY_plog;
  int TotPal, TotPalA, TotPalB, TotPresa;
  int PianiInPresa = PIANIINPRESA;
  TColor ColTGV;
  TColor ColPLOGICO;
  TColor ColBOX_A;
  TColor ColBOX_B;
  TColor ColPEN_A;
  TColor ColPEN_B;
  TBrushStyle StlBOX;
  TPoint points[5];
  TPoint pointA[4];
  TPoint pointB[4];
  long double sinA, cosA, sinB, cosB,  sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double grd, grd_su_nodo, radA, radB, radC, radB1, radB2;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double ComponenteXOffset,  ComponenteYOffset;
  long double ComponenteX1Offset,  ComponenteX2Offset, ComponenteY1Offset, ComponenteY2Offset;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  long double diff, off_pixelX, off_pixelY;
  long double offsY, offsX, hip;
  double grd_ly, Xly, Yly;
  int tipo_carico     = TIPO_CARICO_TGV;
  int carico_in_punta = CARICO_IN_PUNTA;
  char buffer[101];

  nod       = 0;
  //-------------------------------------------------------
  // Calcolo delle dimensioni grafiche del TGV
  //-------------------------------------------------------
  lung_agv  = LUNG_AGV;
  larg_agv  = LARG_AGV;
  lung_cest = LUNG_CEST;
  larg_cest = LARG_CEST;
  //-------------------------------------------------------
  // Se la posizione del TGV==0 allora --> Skip!
  //-------------------------------------------------------
  if(AGV[Num]->stato.pos<1           ) return;
  if(AGV[Num]->stato.pos>TOTPUNTI    ) return;
  //-------------------------------------------------------
  // Se la in nodo della posizione non ha pagina --> Skip!
  //-------------------------------------------------------
  if(Punti[AGV[Num]->stato.pos][4]==0) return;
  //-------------------------------------------------------
  // TGV TRASPARENTI
  // Per tutte le navette laser è necessario attivare
  // la visualizzazione trasparente (richiesta da Annovi)
  //-------------------------------------------------------
//if(AGV[Num]->mission.GestioneLaser==true && LivelloPassword>2) VisualizzaTgvTrasparente[Num]=true;
  //-------------------------------------------------------
  // Inizializzazione colorazioni
  //-------------------------------------------------------
  ColBOX_A    = clSFONDO;
  ColBOX_B    = clSFONDO;
  ColTGV      = AGV[Num]->mission.Col;
  ColPLOGICO  = clWhite;
  //-------------------------------------------------------
  // Recupero stato TGV
  //-------------------------------------------------------
  if((AGV[Num]->stato.pos>0 ) && (AGV[Num]->stato.pos<=TOTPUNTI)) nod=AGV[Num]->stato.pos;  // Numero nodo attuale o ultimo
  su_nod   = AGV[Num]->stato.s.bit.sunod;      // AGV su nodo (1)
  indietro = AGV[Num]->stato.s.bit.ind;        // (1=ind, 0=null)
  avanti   = AGV[Num]->stato.s.bit.av;         // (1=av,  0=null)
  carA     = AGV[Num]->stato.s.bit.carA;       // AGV con carico (ALTO)
  carB     = AGV[Num]->stato.s.bit.carB;       // AGV con carico (BASSO)
  if(carA || carB){
     DisegnaBox(AGV[Num]->mission.BoxA.StatoLav, AGV[Num]->mission.BoxB.StatoLav, &ColBOX_A, &ColBOX_B);
     //---------------------------------------------
     // Contorno BOX sempre nero tranne in caso
     // di visulizzazione impianto o BOX BLOCCATI
     //---------------------------------------------
     ColPEN_A = clSFONDO;
     ColPEN_B = clSFONDO;
     if(VisualizzaTgvTrasparente[Num] || VisualizzaZoneImpianto){
        ColPEN_A = ColBOX_A;
        ColPEN_B = ColBOX_B;
     }
  }

  //-------------------------------------------------------
  // Se comunicazione interrotta recupera
  //-------------------------------------------------------
  xcen=0; ycen=0; grd=0;
  xcen = ((Punti[nod][0]-offset[1][0]));        // pixel x del nodo occupato
  ycen = ((Punti[nod][1]-offset[1][1]));        // pixel y del nodo occupato

  //-------------------------------------------------------
  // Recupero ROTAZIONE
  //-------------------------------------------------------
  grd = AGV[Num]->stato.rot_calamite;
  if(AGV[Num]->mission.GestioneLaser==true) grd = AGV[Num]->stato.rot_laser;
  pixel_da_nodo  = AGV[Num]->stato.offset_da_nodo * AGV[Num]->mission.RapEncoder;

  //-------------------------------------------------------
  // (TEST DELLE ROTAZIONI)
  // Recupero gradi da edit di test rotazioni
  //-------------------------------------------------------
  if(EditGrd->Visible==true){
     grd = atof(EditGrd->Text.c_str());
     pixel_da_nodo = atof(EditOff->Text.c_str()) * AGV[Num]->mission.RapEncoder;
  }
  //-------------------------------------------------------
  // GESTIONE LASER
  // Recupera posizione dal Laser
  //-------------------------------------------------------
  if(AGV[Num]->mission.GestioneLaser && ((!AGV[Num]->mission.EsclPLC && !break_Com[COM1]) || (IdPC>1 && AGV[Num]->stato.x_laser!=0 && AGV[Num]->stato.y_laser!=0))){
     AGV[Num]->convertiXY_laser_to_layout(AGV[Num]->stato.x_laser/10, AGV[Num]->stato.y_laser/10,  &Xly, &Yly);
     AGV[Num]->convertiGRD_laser_to_layout(AGV[Num]->stato.rot_laser, &grd_ly);
     xcen= (int) Xly;
     ycen= (int) Yly;
     xcen= xcen-offset[1][0];      // pixel x del nodo occupato
     ycen= ycen-offset[1][1];      // pixel y del nodo occupato
     grd = (long double) grd_ly;
     if( grd>358            ) grd=0;
     if( grd>0   && grd<2   ) grd=0;
     if( grd>88  && grd<92  ) grd=90;
     if( grd>178 && grd<182 ) grd=180;
     if( grd>268 && grd<272 ) grd=270;
  }
  //-------------------------------------------------------
  // GESTIONE A CALAMITE
  // Pixel da nodo
  //-------------------------------------------------------
  else{
     //------------------------------------------
     // 30/11/95 Recupera direzione precedente
     //------------------------------------------
     if( su_nod   ) { PrecAvanti[Num] = 0; PrecIndietro[Num] =0;}
     if( avanti   ) { PrecAvanti[Num] = 1; PrecIndietro[Num] =0;}
     if( indietro ) { PrecAvanti[Num] = 0; PrecIndietro[Num] =1;}
     avanti   = avanti   || PrecAvanti[Num];
     indietro = indietro || PrecIndietro[Num];
     //------------------------------------------
     // Aggiorno i gradi di inizio rotazione
     // solo finchè varia l'offset da nodo
     //------------------------------------------
     if(PrecOffDaNod[Num]!=pixel_da_nodo || su_nod){
        PrecGrdSuNod[Num] = grd;
        PrecOffDaNod[Num] = pixel_da_nodo;
     }
     //------------------------------------------
     // Calcolo coordinate centro TGV in base
     // all'offset da nodo
     //------------------------------------------
     if( !su_nod ){
        //-----------------------------------------------------------------
        // Recupero i gradi a inizio rotazione
        //-----------------------------------------------------------------
        grd_su_nodo = PrecGrdSuNod[Num];
        //-----------------------------------------------------------------
        // **** modifica del 24-5-95 : nel caso in cui sia alto il flag
        //      di riposizionamento ingresso magazzino, allora la distanza
        //      dal nodo deve essere negata, in quanto l'AGV si st… avvi-
        //      cinando al Nodo e non allontanando come negli altri casi.
        //
        //-----------------------------------------------------------------
        if( AGV[Num]->stato.s.bit.riposiz == 1 ){
           pixel_da_nodo = -pixel_da_nodo;
        }
        //-----------------------------------------------------------------
        // Modifico coordinate centro TGV in base al verso di movimento
        //-----------------------------------------------------------------
        radA = DegToRad(grd_su_nodo);
        SinCos(radA, sinA, cosA);

        off_pixelX = pixel_da_nodo*sinA;
        off_pixelY = pixel_da_nodo*cosA;

        if(indietro){
           xcen = xcen + off_pixelX;
           ycen = ycen + off_pixelY;
        }
        if(avanti){
           xcen = xcen - off_pixelX;
           ycen = ycen - off_pixelY;
        }
     }
  }

  //----------------------------------------------------
  // Se riserva batteria allora Lampeggiante !
  //----------------------------------------------------
  if( AGV[Num]->stato.s.bit.ris && Lampeggio ){
     ColTGV     = clSFONDO;
     ColBOX_A   = clSFONDO;
     ColBOX_B   = clSFONDO;
     ColPLOGICO = clSFONDO;
  }

  //----------------------------------------------------
  // Assegna il colore al TGV
  //----------------------------------------------------
  Sfondo->Canvas->Brush->Style = bsSolid;
  Sfondo->Canvas->Pen->Color   = ColTGV;
  Sfondo->Canvas->Brush->Color = ColTGV;

  //----------------------------------------
  // Radianti e gardi TGV
  //----------------------------------------
  radA  =  DegToRad(grd);
  SinCos(radA, sinA, cosA);

  //----------------------------------------
  // Calcolo offset punto logico riferito
  // al centro navetta grafica in "X"
  //----------------------------------------
  xlog = xcen;
  ylog = ycen;
  offsetX_plog = ADJUST_GRAFICA_TGV*sinA;
  offsetY_plog = ADJUST_GRAFICA_TGV*cosA;
  xcen = xlog + offsetX_plog;
  ycen = ylog + offsetY_plog;

  //----------------------------------------
  // Disegno TGV
  // Con calcolo  Angolo orientamento TGV
  //----------------------------------------
  MezzoLatoX = (long double)(larg_agv/2);
  MezzoLatoY = (long double)(lung_agv/2);

  radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

  radB1 =  radA + radB;
  radB2 =  radA - radB;
  SinCos(radB1, sinB1, cosB1);
  SinCos(radB2, sinB2, cosB2);

  MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

  ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
  ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
  ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
  ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

  xtgv1 = (int) (xcen-(int)(ComponenteX1));
  xtgv2 = (int) (xcen+(int)(ComponenteX2));
  xtgv3 = (int) (xcen+(int)(ComponenteX1));
  xtgv4 = (int) (xcen-(int)(ComponenteX2));

  ytgv1 = (int) (ycen+(int)(ComponenteY1));
  ytgv2 = (int) (ycen-(int)(ComponenteY2));
  ytgv3 = (int) (ycen-(int)(ComponenteY1));
  ytgv4 = (int) (ycen+(int)(ComponenteY2));

  xlog1 = (int) (xlog-(int)(ComponenteX1));
  xlog2 = (int) (xlog+(int)(ComponenteX2));
  xlog3 = (int) (xlog+(int)(ComponenteX1));
  xlog4 = (int) (xlog-(int)(ComponenteX2));

  ylog1 = (int) (ylog+(int)(ComponenteY1));
  ylog2 = (int) (ylog-(int)(ComponenteY2));
  ylog3 = (int) (ylog-(int)(ComponenteY1));
  ylog4 = (int) (ylog+(int)(ComponenteY2));

  Pos_Agv[Num].xc=xcen/Rap*Rid;
  Pos_Agv[Num].yc=ycen/Rap*Rid;

  //-----------------------------------------------
  // Aggiustamenti dovuti ad arrotondamenti
  //-----------------------------------------------
  if(grd==90 || grd==270){
     offs1 = abs(xtgv4-xtgv3);
     if(offs1<3) xtgv4=xtgv3;
     offs1 = abs(xtgv2-xtgv1);
     if(offs1<3) xtgv2=xtgv1;
     offs1 = abs(ytgv4-ytgv1);
     if(offs1<3) ytgv4=ytgv1;
     offs1 = abs(ytgv3-ytgv2);
     if(offs1<3) ytgv3=ytgv2;
  }

  if(grd==0 || grd==180){
     offs1 = abs(xtgv4-xtgv1);
     if(offs1<3) xtgv4=xtgv1;
     offs1 = abs(xtgv3-xtgv2);
     if(offs1<3) xtgv3=xtgv2;
     offs1 = abs(ytgv2-ytgv1);
     if(offs1<3) ytgv2=ytgv1;
     offs1 = abs(ytgv4-ytgv3);
     if(offs1<3) ytgv4=ytgv3;
  }

  points[0] = Point(xtgv1/Rap*Rid, ytgv1/Rap*Rid);
  points[1] = Point(xtgv2/Rap*Rid, ytgv2/Rap*Rid);
  points[2] = Point(xtgv3/Rap*Rid, ytgv3/Rap*Rid);
  points[3] = Point(xtgv4/Rap*Rid, ytgv4/Rap*Rid);

  // --- coordinate del punto logico
  Xl1 =  xlog1;
  Xl2 =  xlog2;
  Xl3 =  xlog3;
  Xl4 =  xlog4;
  Yl1 =  ylog1;
  Yl2 =  ylog2;
  Yl3 =  ylog3;
  Yl4 =  ylog4;

  // Se ANNOVI in DEBUG visualizza Trasparente
  if(VisualizzaTgvTrasparente[Num] || VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear ;

  Sfondo->Canvas->Polygon(points, 3);

  //-------------------------------------------
  // Disegna bumper
  //-------------------------------------------
  if(Rid>=0.25){
/*
     xb11 = (int) (xcen-(int)(ComponenteX1));
     xb14 = (int) (xcen+(int)(ComponenteX2));

     yb11 = (int) (ycen+(int)(ComponenteY1));
     yb14 = (int) (ycen-(int)(ComponenteY2));

     MezzoLatoX = (long double)((larg_agv/2)-10);
     MezzoLatoY = (long double)((lung_agv/2)+50);

     radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

     xb12 = (int) (xcen-(int)(ComponenteX1));
     xb13 = (int) (xcen+(int)(ComponenteX2));

     yb12 = (int) (ycen+(int)(ComponenteY1));
     yb13 = (int) (ycen-(int)(ComponenteY2));

     //-----------------------------------------------
     // Disegno linee bumper (inferiore con TGV a 0°)
     //-----------------------------------------------
     points[0] = Point(xb11/Rap*Rid, yb11/Rap*Rid);
     points[1] = Point(xb12/Rap*Rid, yb12/Rap*Rid);
     points[2] = Point(xb13/Rap*Rid, yb13/Rap*Rid);
     points[3] = Point(xb14/Rap*Rid, yb14/Rap*Rid);
     Sfondo->Canvas->PolyBezier(points, 3);    // bumper con linea curva
  // Sfondo->Canvas->Polyline(points, 3);      // bumper con linea spezzata

     //-----------------------------------------------
     // Riempimento bumper
     //-----------------------------------------------
     MezzoLatoX = (long double)(larg_agv/3);
     MezzoLatoY = (long double)((lung_agv/2)+10);

     radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

     xbr1 = (int) (xcen-(int)(ComponenteX1));
     xbr2 = (int) (xcen+(int)(ComponenteX2));

     ybr1 = (int) (ycen+(int)(ComponenteY1));
     ybr2 = (int) (ycen-(int)(ComponenteY2));


   //Sfondo->Canvas->Pen->Color = clRed;
   //Sfondo->Canvas->MoveTo(xbr1/Rap*Rid, ybr1/Rap*Rid);
   //Sfondo->Canvas->LineTo(xbr2/Rap*Rid, ybr2/Rap*Rid);

     Sfondo->Canvas->FloodFill(xbr1/Rap*Rid, ybr1/Rap*Rid, ColTGV, fsBorder);
     Sfondo->Canvas->FloodFill(xbr2/Rap*Rid, ybr2/Rap*Rid, ColTGV, fsBorder);
*/

     xb11 = xtgv1;
     xb14 = xtgv2;
   //xb11 = (int) (xcen-(int)(ComponenteX1));
   //xb14 = (int) (xcen+(int)(ComponenteX2));

     yb11 = ytgv1;
     yb14 = ytgv2;
   //yb11 = (int) (ycen+(int)(ComponenteY1));
   //yb14 = (int) (ycen-(int)(ComponenteY2));

     MezzoLatoX = (long double)(abs((larg_agv/2)-10));
     MezzoLatoY = (long double)(abs((lung_agv/2)+70));

     radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt_algo((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

     xb12 = (int) (xcen-(int)(ComponenteX1));
     xb13 = (int) (xcen+(int)(ComponenteX2));

     yb12 = (int) (ycen+(int)(ComponenteY1));
     yb13 = (int) (ycen-(int)(ComponenteY2));

     //-----------------------------------------------
     // Disegno linee bumper (inferiore con TGV a 0°)
     //-----------------------------------------------
     points[0] = Point(xb11/Rap*Rid, yb11/Rap*Rid);
     points[1] = Point(xb12/Rap*Rid, yb12/Rap*Rid);
     points[2] = Point(xb13/Rap*Rid, yb13/Rap*Rid);
     points[3] = Point(xb14/Rap*Rid, yb14/Rap*Rid);

     Sfondo->Canvas->Pen->Width = 2;

     Sfondo->Canvas->PolyBezier(points, 3);    // bumper con linea curva
  // Sfondo->Canvas->Polyline(points, 3);      // bumper con linea spezzata

     Sfondo->Canvas->Pen->Width = 1;

     //-----------------------------------------------
     // Riempimento bumper
     //-----------------------------------------------
     MezzoLatoX = (long double)(larg_agv/3);
     MezzoLatoY = (long double)((lung_agv/2)+10);

     radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt_algo((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

     xbr1 = (int) ((xcen-(int)(ComponenteX1))/Rap*Rid);
     xbr2 = (int) ((xcen+(int)(ComponenteX2))/Rap*Rid);

     ybr1 = (int) ((ycen+(int)(ComponenteY1))/Rap*Rid);
     ybr2 = (int) ((ycen-(int)(ComponenteY2))/Rap*Rid);

     Sfondo->Canvas->FloodFill(xbr1, ybr1, ColTGV, fsBorder);
     Sfondo->Canvas->FloodFill(xbr2, ybr2, ColTGV, fsBorder);
  }

  //-------------------------------------------------------
  // Disegno POSTAZIONE PALLET
  // Rettangolo nero su quale verrà disegnata la paletta
  // delle dimensioni reali
  //-------------------------------------------------------
  mezzo_lung = LUNG_AREA_CARICO/2;
  mezzo_larg = LARG_AREA_CARICO/2;
  //-------------------------------------------------------
  // OffSet di posizionamento RIQUADRO GRUPPO FORCHE
  //-------------------------------------------------------
  offs = (lung_agv/2) - mezzo_lung;

  //-------------------------------------------------------
  // Calcolo coordinate grafiche
  //-------------------------------------------------------
  ComponenteXOffset = -offs*sinA;
  ComponenteYOffset = -offs*cosA;

  MezzoLatoX = (long double)(mezzo_larg);
  MezzoLatoY = (long double)(mezzo_lung);

  radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
  radB1 =  radA + radB;
  radB2 =  radA - radB;
  SinCos(radB1, sinB1, cosB1);
  SinCos(radB2, sinB2, cosB2);

  MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

  ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
  ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
  ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
  ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

  xs1 = (int) (xcen-(int)(ComponenteX1) +(int)(ComponenteXOffset));
  xs2 = (int) (xcen+(int)(ComponenteX2) +(int)(ComponenteXOffset));
  xs3 = (int) (xcen+(int)(ComponenteX1) +(int)(ComponenteXOffset));
  xs4 = (int) (xcen-(int)(ComponenteX2) +(int)(ComponenteXOffset));

  ys1 = (int) (ycen+(int)(ComponenteY1) +(int)(ComponenteYOffset));
  ys2 = (int) (ycen-(int)(ComponenteY2) +(int)(ComponenteYOffset));
  ys3 = (int) (ycen-(int)(ComponenteY1) +(int)(ComponenteYOffset));
  ys4 = (int) (ycen+(int)(ComponenteY2) +(int)(ComponenteYOffset));

  //-----------------------------------------------
  // Aggiustamenti dovuti ad arrotondamenti
  //-----------------------------------------------
  if(grd==90 || grd==270){
     offs1 = abs(xs4-xs3);
     if(offs1<3) xs4=xs3;
     offs1 = abs(xs2-xs1);
     if(offs1<3) xs2=xs1;
     offs1 = abs(ys4-ys1);
     if(offs1<3) ys4=ys1;
     offs1 = abs(ys3-ys2);
     if(offs1<3) ys3=ys2;
  }
  if(grd==0 || grd==180){
     offs1 = abs(xs4-xs1);
     if(offs1<3) xs4=xs1;
     offs1 = abs(xs3-xs2);
     if(offs1<3) xs3=xs2;
     offs1 = abs(ys2-ys1);
     if(offs1<3) ys2=ys1;
     offs1 = abs(ys4-ys3);
     if(offs1<3) ys4=ys3;
  }
  // --- disegno riquadro pallet
  points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
  points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
  points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
  points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RRRRRRRRRRRRRRRRRRRRRR
    int myDist;
    if(xs1<=xs2 && xs1<=xs3 && xs1<=xs4) Pos_Agv[Num].x1=xs1;
    if(xs2<=xs1 && xs2<=xs3 && xs2<=xs4) Pos_Agv[Num].x1=xs2;
    if(xs3<=xs1 && xs3<=xs2 && xs3<=xs4) Pos_Agv[Num].x1=xs3;
    if(xs4<=xs1 && xs4<=xs2 && xs4<=xs3) Pos_Agv[Num].x1=xs4;

    if(xs1>=xs2 && xs1>=xs3 && xs1>=xs4) Pos_Agv[Num].x2=xs1;
    if(xs2>=xs1 && xs2>=xs3 && xs2>=xs4) Pos_Agv[Num].x2=xs2;
    if(xs3>=xs1 && xs3>=xs2 && xs3>=xs4) Pos_Agv[Num].x2=xs3;
    if(xs4>=xs1 && xs4>=xs2 && xs4>=xs3) Pos_Agv[Num].x2=xs4;

    if(ys1<=ys2 && ys1<=ys3 && ys1<=ys4) Pos_Agv[Num].y1=ys1;
    if(ys2<=ys1 && ys2<=ys3 && ys2<=ys4) Pos_Agv[Num].y1=ys2;
    if(ys3<=ys1 && ys3<=ys2 && ys3<=ys4) Pos_Agv[Num].y1=ys3;
    if(ys4<=ys1 && ys4<=ys2 && ys4<=ys3) Pos_Agv[Num].y1=ys4;

    if(ys1>=ys2 && ys1>=ys3 && ys1>=ys4) Pos_Agv[Num].y2=ys1;
    if(ys2>=ys1 && ys2>=ys3 && ys2>=ys4) Pos_Agv[Num].y2=ys2;
    if(ys3>=ys1 && ys3>=ys2 && ys3>=ys4) Pos_Agv[Num].y2=ys3;
    if(ys4>=ys1 && ys4>=ys2 && ys4>=ys3) Pos_Agv[Num].y2=ys4;

    Pos_Agv[Num].x1=(Pos_Agv[Num].x1/Rap*Rid);
    Pos_Agv[Num].x2=(Pos_Agv[Num].x2/Rap*Rid);
    Pos_Agv[Num].y1=(Pos_Agv[Num].y1/Rap*Rid);
    Pos_Agv[Num].y2=(Pos_Agv[Num].y2/Rap*Rid);

    myDist=Pos_Agv[Num].x2-Pos_Agv[Num].x1;
    myDist=myDist/4;
    Pos_Agv[Num].x1=Pos_Agv[Num].x1+myDist;
    Pos_Agv[Num].x2=Pos_Agv[Num].x2-myDist;

    myDist=Pos_Agv[Num].y2-Pos_Agv[Num].y1;
    myDist=myDist/4;
    Pos_Agv[Num].y1=Pos_Agv[Num].y1+myDist;
    Pos_Agv[Num].y2=Pos_Agv[Num].y2-myDist;

//  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RRRRRRRRRRRRRRRRRRRRRR

  //-------------------------------------------
  // Disegno PALLET A BORDO ( Nero se vuoto )
  //-------------------------------------------
  StlBOX = bsSolid;
  // Se ANNOVI in DEBUG visualizza Trasparente
  if(VisualizzaZoneImpianto || VisualizzaTgvTrasparente[Num]){
     StlBOX     = bsClear;
     ColPLOGICO = ColTGV;
  }
  //------------------------------------------
  // Rettangolo nero TGV scarico
  //------------------------------------------
  Sfondo->Canvas->Pen->Color   = clSFONDO;                       // sfondo NERO per il Pallet
  Sfondo->Canvas->Brush->Color = clSFONDO;
  Sfondo->Canvas->Brush->Style = StlBOX;
  //------------------------------------------
  // Se la dimensione del carico supera la
  // dimensione del TGV, faccio un riquadro
  // esterno
  //------------------------------------------
  if(larg_agv<=LARG_AREA_CARICO || lung_agv<=LUNG_AREA_CARICO){
     Sfondo->Canvas->Pen->Color = ColTGV;
  }

  //------------------------------------------
  // Disegno riquadro di carico
  //------------------------------------------
  Sfondo->Canvas->Polygon(points, 3);

  //------------------------------------------
  // CORREZIONE GRAFICA
  // Se il pallette è nell'ingombro del TGV
  // traccio una riga nera per la coorrezione
  // grafica dovuta all'arrotondamento pixel
  //------------------------------------------
  if(larg_agv>LARG_AREA_CARICO && lung_agv>LUNG_AREA_CARICO){
     Sfondo->Canvas->Pen->Color = clSFONDO;
     Sfondo->Canvas->Pen->Width = 4;
     Sfondo->Canvas->MoveTo( xs3/Rap*Rid, ys3/Rap*Rid );
     Sfondo->Canvas->LineTo( xs4/Rap*Rid, ys4/Rap*Rid );
     Sfondo->Canvas->Pen->Width = 1;
  }

  //------------------------------------------
  // DISEGNA PALETTA BORDO
  // Dimensioni e colore in base al tipo box
  // e al prodotto contenuto
  //------------------------------------------
  if(carA || carB){
     //-------------------------------------------------------
     // Recupero le dimensioni del pallet dalla struttura
     // TIPIBOX
     //-------------------------------------------------------
     if(AGV[Num]->mission.PresCEST_B!=LIBERA){    // FORCHE "B"
        lung_cest = M.TipiBox[AGV[Num]->mission.BoxB.TipoBox].Lung;
        larg_cest = M.TipiBox[AGV[Num]->mission.BoxB.TipoBox].Larg;
     }
     else{                                        // FORCHE "A"
        lung_cest = M.TipiBox[AGV[Num]->mission.BoxA.TipoBox].Lung;
        larg_cest = M.TipiBox[AGV[Num]->mission.BoxA.TipoBox].Larg;
     }
     //-------------------------------------------------------
     // Dimensiono la presa in base alla presenza di peso
     // maggiore
     //
     // 12/03/2014 - visualizzazione dei pallet nella loro
     // reale posizione, non centrati al TGV.  
     //-------------------------------------------------------
     TotPresa=POSINPRESA;
   //TotPresa=0;
   //for(i=POSINPRESA; i>0; i--){
   //   if(AGV[Num]->mission.BoxB.Presa[i-1][0]==false && AGV[Num]->mission.BoxA.Presa[i-1][0]==false) continue;
   //   TotPresa=i;
   //   break;
   //}
     //-------------------------------------------------------
     // Disegno di ogni box previsto in presa
     //-------------------------------------------------------
     for(i=0; i<TotPresa; i++){
        //-------------------------------------------------------
        // Calcolo formato grafico
        //-------------------------------------------------------
        mezzo_lung = lung_cest/2;
        mezzo_larg = larg_cest/2;
        //-------------------------------------------------------
        // OffSet di posizionamento PALETTA
        //-------------------------------------------------------
        offsX=offsY=0;
        if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY = (lung_agv/2) - mezzo_lung;
        else{
           offsY = ((lung_agv/2) - mezzo_lung) + (lung_cest-LUNG_AREA_CARICO);
        }
        //-------------------------------------------------------
        // Calcolo coordinate grafiche del centro paletta
        //-------------------------------------------------------
        offsX = (((larg_cest * (TotPresa-i))/2) - mezzo_larg) - (i*(mezzo_larg));
        //-------------------------------------------------------
        // ipotenusa del triangolo rettangolo i cui lati sono
        // gli offset del centro paletta dal centro navetta
        //-------------------------------------------------------
        radB  = (long double)(atanl( offsX / offsY ));
        SinCos(radB, sinB, cosB);
        hip   = offsY / cosB;
        //-------------------------------------------------------
        // Calcolo nell'angolo di rotazione del centro paletta
        // come sommatoria dei gradi del TGV + l'angolo di
        // rotazione dal centro TGV al centro paletta
        //-------------------------------------------------------
        radC = radA + radB;
        SinCos(radC, sinC, cosC);
        //-------------------------------------------------------
        // Offset in X,Y del centro PALLET rispetto al centro TGV
        //-------------------------------------------------------
        ComponenteYOffset = hip * cosC;
        ComponenteXOffset = hip * sinC;

        xpal = xcen-(int)(ComponenteXOffset);
        ypal = ycen-(int)(ComponenteYOffset);
        //-------------------------------------------------------
        // Costruzione del pallet attorno alle coordinate di
        // centro pallet stesso
        //-------------------------------------------------------
        MezzoLatoX = (long double)(mezzo_larg);
        MezzoLatoY = (long double)(mezzo_lung);

        radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
        //-------------------------------------------------------
        // Per una corretta visualizzazione in modalità BOBINE
        // il quadrato su cui viene costruita, non può essere
        // inclinato come la navetta ma deve essere sempre
        // ortogonale allo schermo per evitare deformazioni
        //-------------------------------------------------------
        switch(tipo_carico){
           case  0: radB1 = radA + radB;       // rettangolo
                    radB2 = radA - radB;
                    break;
           case  1: radB1 =    0 + radB;       // ellisse
                    radB2 =    0 - radB;
                    break;
           default: radB1 = radA + radB;       // rettangolo
                    radB2 = radA - radB;
                    break;
        }

        SinCos(radB1, sinB1, cosB1);
        SinCos(radB2, sinB2, cosB2);

        MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

        ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
        ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
        ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
        ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

        xs1 = (int) (xpal-(int)(ComponenteX1));
        xs2 = (int) (xpal+(int)(ComponenteX2));
        xs3 = (int) (xpal+(int)(ComponenteX1));
        xs4 = (int) (xpal-(int)(ComponenteX2));

        ys1 = (int) (ypal+(int)(ComponenteY1));
        ys2 = (int) (ypal-(int)(ComponenteY2));
        ys3 = (int) (ypal-(int)(ComponenteY1));
        ys4 = (int) (ypal+(int)(ComponenteY2));
        //-----------------------------------------------
        // Aggiustamenti dovuti ad arrotondamenti
        //-----------------------------------------------
        if(grd==90 || grd==270){
           offs1 = abs(xs4-xs3);
           if(offs1<3) xs4=xs3;
           offs1 = abs(xs2-xs1);
           if(offs1<3) xs2=xs1;
           offs1 = abs(ys4-ys1);
           if(offs1<3) ys4=ys1;
           offs1 = abs(ys3-ys2);
           if(offs1<3) ys3=ys2;
        }
        if(grd==0 || grd==180){
           offs1 = abs(xs4-xs1);
           if(offs1<3) xs4=xs1;
           offs1 = abs(xs3-xs2);
           if(offs1<3) xs3=xs2;
           offs1 = abs(ys2-ys1);
           if(offs1<3) ys2=ys1;
           offs1 = abs(ys4-ys3);
           if(offs1<3) ys4=ys3;
        }

        // --- disegno riquadro pallet con Forca Singola
        points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
        points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
        points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
        points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

        // --- disegno riquadro pallet con Doppia Forca
        pointA[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
        pointA[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
        pointA[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);

        pointB[0] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
        pointB[1] = Point(xs4/Rap*Rid, ys4/Rap*Rid);
        pointB[2] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
        //-------------------------------------------
        // Disegno PALLET A BORDO
        //-------------------------------------------
        StlBOX = bsSolid;
        // Se ANNOVI in DEBUG visualizza Trasparente
        if(VisualizzaZoneImpianto || VisualizzaTgvTrasparente[Num]) StlBOX = bsClear;
        //-----------------------------------------------
        // Verifico se in ogni posizione di presa ci
        // sono 1 o più pallet
        //-----------------------------------------------
        TotPalA=TotPalB=0;
        for(j=0; j<PIANIINPRESA; j++){
           if(AGV[Num]->mission.BoxA.Presa[i][j]==true) TotPalA++;
           if(AGV[Num]->mission.BoxB.Presa[i][j]==true) TotPalB++;
        }
        //--------------------------------------------
        // GESTIONE DOPPIA FORCA
        // Visualizzazione presenza pallet su doppia
        // forca
        //--------------------------------------------
        if(GestioneDoppiaForcaTGV!=0 || PianiInPresa>1){
           //------------------------------------
           // Visualizzazione in base al tipo
           // di carico
           //------------------------------------
           switch(tipo_carico){
              // RETTANGOLO (PALLET)
              case  0: Sfondo->Canvas->Pen->Color = ColPEN_B;            // FORCA "B"
                       if(TotPalB!=0) Sfondo->Canvas->Brush->Color = ColBOX_B;
                       else           Sfondo->Canvas->Brush->Color = clSFONDO;
                       Sfondo->Canvas->Brush->Style = StlBOX;
                       Sfondo->Canvas->Polygon(pointB, 2);
                       // FORCA "A" (reale)
                       // controllo lo stato della seconda forca
                       if(GestioneDoppiaForcaTGV!=0){
                          Sfondo->Canvas->Pen->Color = ColPEN_A;
                          if(TotPalA!=0) Sfondo->Canvas->Brush->Color = ColBOX_A;
                          else           Sfondo->Canvas->Brush->Color = clSFONDO;
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          Sfondo->Canvas->Polygon(pointA, 2);
                       }
                       // FORCA "A" (virtuale)
                       // Se non è attiva la seconda forca, utilizzo la
                       // visualizzazione per i piani in presa
                       else{
                          if(TotPalB>1){
                             Sfondo->Canvas->Pen->Color   = ColPEN_B;
                             Sfondo->Canvas->Brush->Color = ColBOX_B;
                             Sfondo->Canvas->Brush->Style = StlBOX;
                             Sfondo->Canvas->Polygon(pointA, 2);
                          }
                       }
                       //------------------------------------
                       // Croce nera per indica più di 2
                       // pallet sovrapposti
                       //------------------------------------
                       if(TotPalB>1 && TotPalA>1){
                          Sfondo->Canvas->Pen->Color = clSFONDO;
                          Sfondo->Canvas->MoveTo(xs2/Rap*Rid, ys2/Rap*Rid);
                          Sfondo->Canvas->LineTo(xs4/Rap*Rid, ys4/Rap*Rid);
                        //Sfondo->Canvas->MoveTo(xs1/Rap*Rid, ys1/Rap*Rid); // aggiungo solo la riga mancante per completare la croce
                        //Sfondo->Canvas->LineTo(xs3/Rap*Rid, ys3/Rap*Rid);
                       }
                       break;
              // ELLISSE (BOBINA)
              case  1: Sfondo->Canvas->Brush->Color = clSFONDO;
                       if(TotPalB!=0){Sfondo->Canvas->Pen->Color = ColBOX_B; Sfondo->Canvas->Brush->Color = ColBOX_B;}  // FORCA "B"
                       else          {Sfondo->Canvas->Pen->Color = ColBOX_A; Sfondo->Canvas->Brush->Color = ColBOX_A;}  // FORCA "A"
                       Sfondo->Canvas->Brush->Style = StlBOX;
                       // CERCHIO ESTERNO
                       Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                       //------------------------------------------------------
                       // Visualizzazione cerchi concentrici per quante
                       // presenze sono a bordo
                       //------------------------------------------------------
                       // FORCA "B"
                       TotPal = TotPalB+TotPalA;
                       diff   = MezzoLatoX/TotPal;
                       for(j=1; j<TotPal; j++){        // parto da "1" perchè il cerchio esterno è già fatto
                          MezzoLatoX = MezzoLatoX-diff;
                          MezzoLatoY = MezzoLatoY-diff;
                          MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

                          ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
                          ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
                          ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
                          ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

                          xs1 = (int) (xpal-(int)(ComponenteX1));
                          xs2 = (int) (xpal+(int)(ComponenteX2));
                          xs3 = (int) (xpal+(int)(ComponenteX1));
                          xs4 = (int) (xpal-(int)(ComponenteX2));

                          ys1 = (int) (ypal+(int)(ComponenteY1));
                          ys2 = (int) (ypal-(int)(ComponenteY2));
                          ys3 = (int) (ypal-(int)(ComponenteY1));
                          ys4 = (int) (ypal+(int)(ComponenteY2));
                          //-----------------------------------------------
                          // Aggiustamenti dovuti ad arrotondamenti
                          //-----------------------------------------------
                          if(grd==90 || grd==270){
                             offs1 = abs(xs4-xs3);
                             if(offs1<3) xs4=xs3;
                             offs1 = abs(xs2-xs1);
                             if(offs1<3) xs2=xs1;
                             offs1 = abs(ys4-ys1);
                             if(offs1<3) ys4=ys1;
                             offs1 = abs(ys3-ys2);
                             if(offs1<3) ys3=ys2;
                          }
                          if(grd==0 || grd==180){
                             offs1 = abs(xs4-xs1);
                             if(offs1<3) xs4=xs1;
                             offs1 = abs(xs3-xs2);
                             if(offs1<3) xs3=xs2;
                             offs1 = abs(ys2-ys1);
                             if(offs1<3) ys2=ys1;
                             offs1 = abs(ys4-ys3);
                             if(offs1<3) ys4=ys3;
                          }

                          // --- disegno riquadro pallet con Forca Singola
                          points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
                          points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
                          points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
                          points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

                          Sfondo->Canvas->Pen->Color   = clSFONDO;
                          if(j<=TotPalB) Sfondo->Canvas->Brush->Color = ColBOX_B; // FORCA "B"
                          else           Sfondo->Canvas->Brush->Color = ColBOX_A; // FORCA "A"
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          // CERCHIO INTERNO
                          Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                       }
                       break;
           }

        }
        //--------------------------------------------
        // GESTIONE FORCA SINGOLA
        // Visualizzazione presenza pallet singola
        //--------------------------------------------
        else{
           if(TotPalB!=0){
              Sfondo->Canvas->Pen->Color   = ColPEN_B;
              Sfondo->Canvas->Brush->Color = ColBOX_B;
           }
           else{
              Sfondo->Canvas->Pen->Color   = clLINEE;
              Sfondo->Canvas->Brush->Color = clSFONDO;
           }
           Sfondo->Canvas->Brush->Style = StlBOX;
           //------------------------------------
           // Visualizzazione in base al tipo
           // di carico
           //------------------------------------
           switch(tipo_carico){
              case  0: Sfondo->Canvas->Polygon(points, 3);
                       //------------------------------------
                       // Croce nera per indica più di 2
                       // pallet sovrapposti
                       //------------------------------------
                       if(TotPalB>1){
                          Sfondo->Canvas->Pen->Color = clSFONDO;
                          Sfondo->Canvas->MoveTo(xs1/Rap*Rid, ys1/Rap*Rid);
                          Sfondo->Canvas->LineTo(xs3/Rap*Rid, ys3/Rap*Rid);
                          if(TotPalB>2){
                             Sfondo->Canvas->MoveTo(xs2/Rap*Rid, ys2/Rap*Rid);
                             Sfondo->Canvas->LineTo(xs4/Rap*Rid, ys4/Rap*Rid);
                          }
                       }
                       break;
              case  1: // CERCHIO ESTERNO
                       Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                       //------------------------------------------------------
                       // Visualizzazione cerchi concentrici per quante
                       // presenze sono a bordo
                       //------------------------------------------------------
                       // FORCA "B"
                       TotPal = TotPalB;
                       diff   = MezzoLatoX/TotPal;
                       for(j=1; j<TotPal; j++){        // parto da "1" perchè il cerchio esterno è già fatto
                          MezzoLatoX = MezzoLatoX-diff;
                          MezzoLatoY = MezzoLatoY-diff;
                          MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

                          ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
                          ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
                          ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
                          ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

                          xs1 = (int) (xpal-(int)(ComponenteX1));
                          xs2 = (int) (xpal+(int)(ComponenteX2));
                          xs3 = (int) (xpal+(int)(ComponenteX1));
                          xs4 = (int) (xpal-(int)(ComponenteX2));

                          ys1 = (int) (ypal+(int)(ComponenteY1));
                          ys2 = (int) (ypal-(int)(ComponenteY2));
                          ys3 = (int) (ypal-(int)(ComponenteY1));
                          ys4 = (int) (ypal+(int)(ComponenteY2));
                          //-----------------------------------------------
                          // Aggiustamenti dovuti ad arrotondamenti
                          //-----------------------------------------------
                          if(grd==90 || grd==270){
                             offs1 = abs(xs4-xs3);
                             if(offs1<3) xs4=xs3;
                             offs1 = abs(xs2-xs1);
                             if(offs1<3) xs2=xs1;
                             offs1 = abs(ys4-ys1);
                             if(offs1<3) ys4=ys1;
                             offs1 = abs(ys3-ys2);
                             if(offs1<3) ys3=ys2;
                          }
                          if(grd==0 || grd==180){
                             offs1 = abs(xs4-xs1);
                             if(offs1<3) xs4=xs1;
                             offs1 = abs(xs3-xs2);
                             if(offs1<3) xs3=xs2;
                             offs1 = abs(ys2-ys1);
                             if(offs1<3) ys2=ys1;
                             offs1 = abs(ys4-ys3);
                             if(offs1<3) ys4=ys3;
                          }

                          // --- disegno riquadro pallet con Forca Singola
                          points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
                          points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
                          points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
                          points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

                          Sfondo->Canvas->Pen->Color   = clLINEE;
                          Sfondo->Canvas->Brush->Color = ColBOX_B; // FORCA "B"
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          // CERCHIO INTERNO
                          Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                       }
                       break;
              default: Sfondo->Canvas->Polygon(points, 3);
                       //------------------------------------
                       // Croce nera per indica più di 2
                       // pallet sovrapposti
                       //------------------------------------
                       if(TotPalB>1){
                          Sfondo->Canvas->Pen->Color = clSFONDO;
                          Sfondo->Canvas->MoveTo(xs1/Rap*Rid, ys1/Rap*Rid);
                          Sfondo->Canvas->LineTo(xs3/Rap*Rid, ys3/Rap*Rid);
                          if(TotPalB>2){
                             Sfondo->Canvas->MoveTo(xs2/Rap*Rid, ys2/Rap*Rid);
                             Sfondo->Canvas->LineTo(xs4/Rap*Rid, ys4/Rap*Rid);
                          }
                       }
                       break;
           }
        }
     }
  }

//Sfondo->Canvas->Pen->Color   = clFuchsia;           // sfondo NERO per il Pallet
//Sfondo->Canvas->Polyline(lines, 1);

  //-------------------------------------------
  // Disegno Centro PUNTO LOGICO
  //-------------------------------------------
  Sfondo->Canvas->Pen->Color = ColPLOGICO;
  xx1 = ((Xl1-Xl2)/2+Xl2);
  yy1 = ((Yl1-Yl2)/2+Yl2);
  xx2 = ((Xl3-Xl4)/2+Xl4);
  yy2 = ((Yl3-Yl4)/2+Yl4);

  xx1 = (xx1-xlog)/4+xlog;
  yy1 = (yy1-ylog)/4+ylog;
  xx2 = (xx2-xlog)/4+xlog;
  yy2 = (yy2-ylog)/4+ylog;


  Sfondo->Canvas->MoveTo( xx1/Rap*Rid, yy1/Rap*Rid );
  Sfondo->Canvas->LineTo( xx2/Rap*Rid, yy2/Rap*Rid );

  xx1 = ((Xl2-Xl3)/2+Xl3);
  yy1 = ((Yl2-Yl3)/2+Yl3);
  xx2 = ((Xl1-Xl4)/2+Xl4);
  yy2 = ((Yl1-Yl4)/2+Yl4);

  xx1 = (xx1-xlog)/4+xlog;
  yy1 = (yy1-ylog)/4+ylog;
  xx2 = (xx2-xlog)/4+xlog;
  yy2 = (yy2-ylog)/4+ylog;

  Sfondo->Canvas->MoveTo( xx1/Rap*Rid, yy1/Rap*Rid );
  Sfondo->Canvas->LineTo( xx2/Rap*Rid, yy2/Rap*Rid );

  //-----------------------------------------------------------
  // Disegno il numero del TGV
  //-----------------------------------------------------------
  if(Rid>=RID_NUMERI){
     if(VisualizzaTgvTrasparente[Num]==false){
        xx1 = ((Xl1-Xl2)/1+Xl2);
        if(Xl1>Xl2) xx1 = (Xl1);
        if(Xl1<Xl2) xx1 = (Xl2);
        yy1 = ((Yl1-Yl2)/2+Yl2);
        if(Yl1<Yl2) yy1 = (Yl1);
        if(Yl1>Yl2) yy1 = (Yl2);

        xx1 = (xx1-xlog)/2+xlog;
        yy1 = (yy1-ylog)/2+ylog;

        sprintf(buffer, "%02d", Num);
        StampNum(FormLayout->Sfondo, xx1/Rap*Rid, yy1/Rap*Rid, buffer, clBlack, FALSE, 1);
     }
  }

  // Se ANNOVI in DEBUG visualizza Trasparente
  if(VisualizzaTgvTrasparente[Num]){
     Sfondo->Canvas->Pen->Color   = clSFONDO;
     Sfondo->Canvas->MoveTo( Xl3/Rap*Rid, Yl3/Rap*Rid );
     Sfondo->Canvas->LineTo( Xl4/Rap*Rid, Yl4/Rap*Rid );
  }

  //-------------------------------------------
  // Con TGV IN ALLARME e NON IN RISERVA
  // lampeggia di rosso la cornice esterna
  //-------------------------------------------
  if((AGV[Num]->stato.s.bit.allarme && !AGV[Num]->stato.s.bit.ris) || AGV[Num]->allarme_interno){
     //-----------------------------------------
     // contorno la superficie di tutto il TGV
     //-----------------------------------------
     larg_all = larg_agv;
     lung_all = lung_agv;
     if(larg_agv<LARG_AREA_CARICO) larg_all = LARG_AREA_CARICO;
     if(lung_agv<LUNG_AREA_CARICO) lung_all = LUNG_AREA_CARICO;
     //-----------------------------------------
     // Allarme lampeggiante
     //-----------------------------------------
     if(!Lampeggio){
        Sfondo->Canvas->Pen->Color   = clRed;
        Sfondo->Canvas->Brush->Color = clRed;
        Sfondo->Canvas->Brush->Style = bsClear;

        radA = DegToRad(grd);

        MezzoLatoX = (long double)((larg_all+100)/2);
        MezzoLatoY = (long double)((lung_all+140)/2);

        radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
        radB1 = radA + radB;
        radB2 = radA - radB;
        SinCos(radB1, sinB1, cosB1);
        SinCos(radB2, sinB2, cosB2);

        MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

        ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
        ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
        ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
        ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

        xs1 = (int) (xcen-(int)(ComponenteX1));
        xs2 = (int) (xcen+(int)(ComponenteX2));
        xs3 = (int) (xcen+(int)(ComponenteX1));
        xs4 = (int) (xcen-(int)(ComponenteX2));

        ys1 = (int) (ycen+(int)(ComponenteY1));
        ys2 = (int) (ycen-(int)(ComponenteY2));
        ys3 = (int) (ycen-(int)(ComponenteY1));
        ys4 = (int) (ycen+(int)(ComponenteY2));

        //-----------------------------------------------
        // Aggiustamenti dovuti ad arrotondamenti
        //-----------------------------------------------
        if(grd==90 || grd==270){
           offs1 = abs(xs4-xs3);
           if(offs1<3) xs4=xs3;
           offs1 = abs(xs2-xs1);
           if(offs1<3) xs2=xs1;
           offs1 = abs(ys4-ys1);
           if(offs1<3) ys4=ys1;
           offs1 = abs(ys3-ys2);
           if(offs1<3) ys3=ys2;
        }
        if(grd==0 || grd==180){
           offs1 = abs(xs4-xs1);
           if(offs1<3) xs4=xs1;
           offs1 = abs(xs3-xs2);
           if(offs1<3) xs3=xs2;
           offs1 = abs(ys2-ys1);
           if(offs1<3) ys2=ys1;
           offs1 = abs(ys4-ys3);
           if(offs1<3) ys4=ys3;
        }

        //-----------------------------------------------
        // Disegno poligono di allarme
        //-----------------------------------------------
        points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
        points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
        points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
        points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

        Sfondo->Canvas->Polygon(points, 3);
     }
  }
}
//---------------------------------------------------------------------------

// ----------------------------------------------
//       disegna_agv_box()
// ----------------------------------------------
// Num = numero AGV (1..MAXAGV)
// Entro nella funzione dopo aver fatto un LOAD_STATO_AGV.
//
//  2) AGV[Num]->stato.offset_da_nodo = distanza da nodo in Impulsi
//     1 Impulso = 4 cm,    spostamento in pixel = (Offset*4)/RAP
//
//  3) Spostamento da nodo: 0ø + Avanti     = X - spostamento
//                          0ø + Indietro   = X + spostamento
//                         90ø + Avanti     = Y - spostamento
//                         90ø + Indietro   = Y + spostamento
//                         AGV su nodo -> spostamento = 0
//
void TFormLayout::disegna_agv_box( int Num )
/***************************************/
{
  int i, j;
  char num_agv[5];
  int lung_agv, larg_agv;
  int lung_all, larg_all;
  int lung_cest, larg_cest;
  int xcen,ycen,xnum,ynum,xlog,ylog,xpal,ypal,xx1,xx2,yy1,yy2;
  int xs1,xs2,ys1,ys2,xs3,ys3,xs4,ys4;
  int Xl1,Xl2,Yl1,Yl2,Xl3,Yl3,Xl4,Yl4;
  int xlog1,xlog2,ylog1,ylog2,xlog3,ylog3,xlog4,ylog4;
  int xtgv1,xtgv2,ytgv1,ytgv2,xtgv3,ytgv3,xtgv4,ytgv4;
  int xb11, xb12, yb11, yb12, xb13, yb13, xb14, yb14;
  int xb21, xb22, yb21, yb22, xb23, yb23, xb24, yb24;
  int nod, su_nod, pixel_da_nodo, avanti, indietro, carA, carB;
  int mezzo_lung, mezzo_larg, offs, offs1;
  int offsetX_plog, offsetY_plog;
//int dist_tra_palette = DIST_CEST_IN_MAG;
  int dist_tra_palette = 0;                 
  int tipo_carico      = TIPO_CARICO_TGV;
  TColor ColTGV;
  TColor ColPLOGICO;
  TColor ColBOX_A;
  TColor ColBOX_B;
  TColor ColPEN_A;
  TColor ColPEN_B;
  TBrushStyle StlBOX;
  TPoint points[5];
  TPoint pointA[4];
  TPoint pointB[4];
  long double sinA, cosA, sinB, cosB,  sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double grd, grd_su_nodo, radA, radB, radC, radB1, radB2;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double ComponenteXOffset,  ComponenteYOffset;
  long double ComponenteX1Offset,  ComponenteX2Offset, ComponenteY1Offset, ComponenteY2Offset;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  long double off_pixelX, off_pixelY;
  long double offsY, offsX, hip;
  double grd_ly, Xly, Yly;

  nod = 0;
  //-------------------------------------------------------
  // Calcolo delle dimensioni grafiche del TGV
  //-------------------------------------------------------
  lung_agv  = LUNG_AGV;
  larg_agv  = LARG_AGV;
  lung_cest = LUNG_CEST;
  larg_cest = LARG_CEST;
  //-------------------------------------------------------
  // Se la posizione del TGV==0 allora --> Skip!
  //-------------------------------------------------------
  if(!AGV[Num]->stato.pos             ) return;
  //-------------------------------------------------------
  // Se la in nodo della posizione non ha pagina --> Skip!
  //-------------------------------------------------------
  if(Punti[AGV[Num]->stato.pos][4]==0) return;
  //-------------------------------------------------------
  // TGV TRASPARENTI
  // Per tutte le navette laser è necessario attivare
  // la visualizzazione trasparente (richiesta da Annovi)
  //-------------------------------------------------------
//if(AGV[Num]->mission.GestioneLaser==true && LivelloPassword>2) VisualizzaTgvTrasparente[Num]=true;
  //-------------------------------------------------------
  // Inizializzazione colorazioni
  //-------------------------------------------------------
  ColBOX_A    = clSFONDO;
  ColBOX_B    = clSFONDO;
  ColTGV      = AGV[Num]->mission.Col;
  ColPLOGICO  = clSFONDO;
  //-------------------------------------------------------
  // Recupero stato TGV
  //-------------------------------------------------------
  if((AGV[Num]->stato.pos>0 ) && (AGV[Num]->stato.pos<=TOTPUNTI)) nod=AGV[Num]->stato.pos;  // Numero nodo attuale o ultimo
  su_nod   = AGV[Num]->stato.s.bit.sunod;      // AGV su nodo (1)
  indietro = AGV[Num]->stato.s.bit.ind;        // (1=ind, 0=null)
  avanti   = AGV[Num]->stato.s.bit.av;         // (1=av,  0=null)
  carA     = AGV[Num]->stato.s.bit.carA;       // AGV con carico (ALTO)
  carB     = AGV[Num]->stato.s.bit.carB;       // AGV con carico (BASSO)
  if(carA || carB){
     DisegnaBox(AGV[Num]->mission.BoxA.StatoLav, AGV[Num]->mission.BoxB.StatoLav, &ColBOX_A, &ColBOX_B);
     //---------------------------------------------
     // Contorno BOX sempre nero tranne in caso
     // di visulizzazione impianto o BOX BLOCCATI
     //---------------------------------------------
     ColPEN_A = clSFONDO;
     ColPEN_B = clSFONDO;
     if(VisualizzaTgvTrasparente[Num] || VisualizzaZoneImpianto){
        ColPEN_A = ColBOX_A;
        ColPEN_B = ColBOX_B;
     }
  }

  //-------------------------------------------------------
  // Se comunicazione interrotta recupera
  //-------------------------------------------------------
  xcen=0; ycen=0; grd=0;
  xcen = ((Punti[nod][0]-offset[1][0]));        // pixel x del nodo occupato
  ycen = ((Punti[nod][1]-offset[1][1]));        // pixel y del nodo occupato

  //-------------------------------------------------------
  // Recupero ROTAZIONE
  //-------------------------------------------------------
  grd = AGV[Num]->stato.rot_calamite;
  if(AGV[Num]->mission.GestioneLaser==true) grd = AGV[Num]->stato.rot_laser;
  pixel_da_nodo  = AGV[Num]->stato.offset_da_nodo * AGV[Num]->mission.RapEncoder;

  //-------------------------------------------------------
  // (TEST DELLE ROTAZIONI)
  // Recupero gradi da edit di test rotazioni
  //-------------------------------------------------------
  if(EditGrd->Visible==true){
     grd = atof(EditGrd->Text.c_str());
     pixel_da_nodo = atof(EditOff->Text.c_str()) * AGV[Num]->mission.RapEncoder;
  }

  //-------------------------------------------------------
  // GESTIONE_ASRV
  // Recupero i gradi dai bit
  //-------------------------------------------------------
  grd_su_nodo=grd;
  #ifdef GESTIONE_ASRV
     if(AGV[Num]->stato.s.bit.grd0  ) grd_su_nodo=0;
     if(AGV[Num]->stato.s.bit.grd90 ) grd_su_nodo=90;
     if(AGV[Num]->stato.s.bit.grd180) grd_su_nodo=180;
     if(AGV[Num]->stato.s.bit.grd270) grd_su_nodo=270;
  #endif;
  //-------------------------------------------------------
  // GESTIONE LASER
  // Recupera posizione dal Laser
  //-------------------------------------------------------
  if(AGV[Num]->mission.GestioneLaser && ((!AGV[Num]->mission.EsclPLC && !break_Com[COM1]) || IdPC>1)){
     AGV[Num]->convertiXY_laser_to_layout(AGV[Num]->stato.x_laser/10, AGV[Num]->stato.y_laser/10,  &Xly, &Yly);
     AGV[Num]->convertiGRD_laser_to_layout(AGV[Num]->stato.rot_laser, &grd_ly);
     xcen= (int) Xly;
     ycen= (int) Yly;
     xcen= xcen-offset[1][0];      // pixel x del nodo occupato
     ycen= ycen-offset[1][1];      // pixel y del nodo occupato
     grd = (long double) grd_ly;
     if( grd>358            ) grd=0;
     if( grd>0   && grd<2   ) grd=0;
     if( grd>88  && grd<92  ) grd=90;
     if( grd>178 && grd<182 ) grd=180;
     if( grd>268 && grd<272 ) grd=270;
  }
  //-------------------------------------------------------
  // GESTIONE A CALAMITE
  // Pixel da nodo
  //-------------------------------------------------------
  else{
     //------------------------------------------
     // 30/11/95 Recupera direzione precedente
     //------------------------------------------
     if( su_nod   ) { PrecAvanti[Num] = 0; PrecIndietro[Num] =0;}
     if( avanti   ) { PrecAvanti[Num] = 1; PrecIndietro[Num] =0;}
     if( indietro ) { PrecAvanti[Num] = 0; PrecIndietro[Num] =1;}
     //------------------------------------------
     // Aggiorno i gradi di inizio rotazione
     // solo finchè varia l'offset da nodo
     //------------------------------------------
     if(PrecOffDaNod[Num]!=pixel_da_nodo || su_nod){
        if(avanti || indietro) PrecGrdSuNod[Num] = grd_su_nodo;
        PrecOffDaNod[Num] = pixel_da_nodo;
     }
     //------------------------------------------
     // Direzione di movimento
     //------------------------------------------
     avanti   = avanti   || PrecAvanti[Num];
     indietro = indietro || PrecIndietro[Num];
     //------------------------------------------
     // Calcolo coordinate centro TGV in base
     // all'offset da nodo
     //------------------------------------------
     if( !su_nod ){
        //-----------------------------------------------------------------
        // Recupero i gradi a inizio rotazione
        //-----------------------------------------------------------------
        grd_su_nodo = PrecGrdSuNod[Num];
        //-----------------------------------------------------------------
        // **** modifica del 24-5-95 : nel caso in cui sia alto il flag
        //      di riposizionamento ingresso magazzino, allora la distanza
        //      dal nodo deve essere negata, in quanto l'AGV si st… avvi-
        //      cinando al Nodo e non allontanando come negli altri casi.
        //
        //-----------------------------------------------------------------
        if( AGV[Num]->stato.s.bit.riposiz == 1 ){
           pixel_da_nodo = -pixel_da_nodo;
        }
        //-----------------------------------------------------------------
        // Modifico coordinate centro TGV in base al verso di movimento
        //-----------------------------------------------------------------
        radA = DegToRad(grd_su_nodo);
        SinCos(radA, sinA, cosA);

        off_pixelX = pixel_da_nodo*sinA;
        off_pixelY = pixel_da_nodo*cosA;

        if(indietro){
           xcen = xcen + off_pixelX;
           ycen = ycen + off_pixelY;
        }
        if(avanti){
           xcen = xcen - off_pixelX;
           ycen = ycen - off_pixelY;
        }
     }
  }

  //----------------------------------------------------
  // Se riserva batteria allora Lampeggiante !
  //----------------------------------------------------
  if( AGV[Num]->stato.s.bit.ris && Lampeggio ){
     ColTGV     = clSFONDO;
     ColBOX_A   = clSFONDO;
     ColBOX_B   = clSFONDO;
     ColPLOGICO = clSFONDO;
  }

  //----------------------------------------------------
  // Assegna il colore al TGV
  //----------------------------------------------------
  Sfondo->Canvas->Brush->Style = bsSolid;
  Sfondo->Canvas->Pen->Color   = ColTGV;
  Sfondo->Canvas->Brush->Color = ColTGV;

  //----------------------------------------
  // Radianti e gardi TGV
  //----------------------------------------
  radA  =  DegToRad(grd);
  SinCos(radA, sinA, cosA);

  //----------------------------------------
  // Calcolo offset punto logico riferito
  // al centro navetta grafica in "X"
  //----------------------------------------
  xlog = xcen;
  ylog = ycen;
  offsetX_plog = ADJUST_GRAFICA_TGV*sinA;
  offsetY_plog = ADJUST_GRAFICA_TGV*cosA;
  xcen = xlog + offsetX_plog;
  ycen = ylog + offsetY_plog;

  //----------------------------------------
  // Disegno TGV
  // Con calcolo  Angolo orientamento TGV
  //----------------------------------------
  MezzoLatoX = (long double)(larg_agv/2);
  MezzoLatoY = (long double)(lung_agv/2);

  radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

  radB1 =  radA + radB;
  radB2 =  radA - radB;
  SinCos(radB1, sinB1, cosB1);
  SinCos(radB2, sinB2, cosB2);

  MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

  ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
  ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
  ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
  ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

  xtgv1 = (int) (xcen-(int)(ComponenteX1));
  xtgv2 = (int) (xcen+(int)(ComponenteX2));
  xtgv3 = (int) (xcen+(int)(ComponenteX1));
  xtgv4 = (int) (xcen-(int)(ComponenteX2));

  ytgv1 = (int) (ycen+(int)(ComponenteY1));
  ytgv2 = (int) (ycen-(int)(ComponenteY2));
  ytgv3 = (int) (ycen-(int)(ComponenteY1));
  ytgv4 = (int) (ycen+(int)(ComponenteY2));

  xlog1 = (int) (xlog-(int)(ComponenteX1));
  xlog2 = (int) (xlog+(int)(ComponenteX2));
  xlog3 = (int) (xlog+(int)(ComponenteX1));
  xlog4 = (int) (xlog-(int)(ComponenteX2));

  ylog1 = (int) (ylog+(int)(ComponenteY1));
  ylog2 = (int) (ylog-(int)(ComponenteY2));
  ylog3 = (int) (ylog-(int)(ComponenteY1));
  ylog4 = (int) (ylog+(int)(ComponenteY2));

  //-----------------------------------------------
  // Puntamento a Editro TGV
  //-----------------------------------------------
  Pos_Agv[Num].xc=xcen/Rap*Rid;
  Pos_Agv[Num].yc=ycen/Rap*Rid;

  //-----------------------------------------------
  // Aggiustamenti dovuti ad arrotondamenti
  //-----------------------------------------------
  if(grd==90 || grd==270){
     offs1 = abs(xtgv4-xtgv3);
     if(offs1<3) xtgv4=xtgv3;
     offs1 = abs(xtgv2-xtgv1);
     if(offs1<3) xtgv2=xtgv1;
     offs1 = abs(ytgv4-ytgv1);
     if(offs1<3) ytgv4=ytgv1;
     offs1 = abs(ytgv3-ytgv2);
     if(offs1<3) ytgv3=ytgv2;
  }

  if(grd==0 || grd==180){
     offs1 = abs(xtgv4-xtgv1);
     if(offs1<3) xtgv4=xtgv1;
     offs1 = abs(xtgv3-xtgv2);
     if(offs1<3) xtgv3=xtgv2;
     offs1 = abs(ytgv2-ytgv1);
     if(offs1<3) ytgv2=ytgv1;
     offs1 = abs(ytgv4-ytgv3);
     if(offs1<3) ytgv4=ytgv3;
  }

  points[0] = Point(xtgv1/Rap*Rid, ytgv1/Rap*Rid);
  points[1] = Point(xtgv2/Rap*Rid, ytgv2/Rap*Rid);
  points[2] = Point(xtgv3/Rap*Rid, ytgv3/Rap*Rid);
  points[3] = Point(xtgv4/Rap*Rid, ytgv4/Rap*Rid);

  // --- coordinate del punto logico
  Xl1 =  xlog1;
  Xl2 =  xlog2;
  Xl3 =  xlog3;
  Xl4 =  xlog4;
  Yl1 =  ylog1;
  Yl2 =  ylog2;
  Yl3 =  ylog3;
  Yl4 =  ylog4;

  // Se ANNOVI in DEBUG visualizza Trasparente
  if(VisualizzaTgvTrasparente[Num] || VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;

  Sfondo->Canvas->Polygon(points, 3);

  //-------------------------------------------
  // Disegna bumper
  //-------------------------------------------
  if(Rid>=0.25){
     MezzoLatoX = (long double)(larg_agv/2);
     MezzoLatoY = (long double)(lung_agv/2);

     radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );



     xb11 = (int) (xcen-(int)(ComponenteX1));
     xb14 = (int) (xcen+(int)(ComponenteX2));
     xb21 = (int) (xcen-(int)(ComponenteX2));
     xb24 = (int) (xcen+(int)(ComponenteX1));

     yb11 = (int) (ycen+(int)(ComponenteY1));
     yb14 = (int) (ycen-(int)(ComponenteY2));
     yb21 = (int) (ycen+(int)(ComponenteY2));
     yb24 = (int) (ycen-(int)(ComponenteY1));

   //MezzoLatoX = (long double)((larg_agv/2)-10);
   //MezzoLatoY = (long double)((lung_agv/2)+20);
     MezzoLatoX = (long double)((larg_agv/2)+20);
     MezzoLatoY = (long double)((lung_agv/2)-10);

     radB  = (long double)(atanl( MezzoLatoY/MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

     xb12 = (int) (xcen-(int)(ComponenteX1));
     xb13 = (int) (xcen+(int)(ComponenteX2));
     xb22 = (int) (xcen-(int)(ComponenteX2));
     xb23 = (int) (xcen+(int)(ComponenteX1));

     yb12 = (int) (ycen+(int)(ComponenteY1));
     yb13 = (int) (ycen-(int)(ComponenteY2));
     yb22 = (int) (ycen+(int)(ComponenteY2));
     yb23 = (int) (ycen-(int)(ComponenteY1));

/*
     //-----------------------------------------------
     // Disegno linee bumper (inferiore con TGV a 0°)
     //-----------------------------------------------
     points[0] = Point(xb11/Rap*Rid, yb11/Rap*Rid);
     points[1] = Point(xb12/Rap*Rid, yb12/Rap*Rid);
     points[2] = Point(xb13/Rap*Rid, yb13/Rap*Rid);
     points[3] = Point(xb14/Rap*Rid, yb14/Rap*Rid);
     Sfondo->Canvas->Polyline(points, 3);      // bumper con linea spezzata

     //-----------------------------------------------
     // Disegno linee bumper (superiore con TGV a 0°)
     //-----------------------------------------------
     points[0] = Point(xb21/Rap*Rid, yb21/Rap*Rid);
     points[1] = Point(xb22/Rap*Rid, yb22/Rap*Rid);
     points[2] = Point(xb23/Rap*Rid, yb23/Rap*Rid);
     points[3] = Point(xb24/Rap*Rid, yb24/Rap*Rid);
     Sfondo->Canvas->Polyline(points, 3);      // bumper con linea spezzata
*/
     points[0] = Point(xb21/Rap*Rid, yb21/Rap*Rid);
     points[1] = Point(xb22/Rap*Rid, yb22/Rap*Rid);
     points[2] = Point(xb12/Rap*Rid, yb12/Rap*Rid);
     points[3] = Point(xb11/Rap*Rid, yb11/Rap*Rid);
     Sfondo->Canvas->Polyline(points, 3);      // bumper con linea spezzata

     points[0] = Point(xb24/Rap*Rid, yb24/Rap*Rid);
     points[1] = Point(xb23/Rap*Rid, yb23/Rap*Rid);
     points[2] = Point(xb13/Rap*Rid, yb13/Rap*Rid);
     points[3] = Point(xb14/Rap*Rid, yb14/Rap*Rid);
     Sfondo->Canvas->Polyline(points, 3);      // bumper con linea spezzata
  }

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RRRRRRRRRRRRRRRRRRRRRR
    int myDist;
    if(xs1<=xs2 && xs1<=xs3 && xs1<=xs4) Pos_Agv[Num].x1=xs1;
    if(xs2<=xs1 && xs2<=xs3 && xs2<=xs4) Pos_Agv[Num].x1=xs2;
    if(xs3<=xs1 && xs3<=xs2 && xs3<=xs4) Pos_Agv[Num].x1=xs3;
    if(xs4<=xs1 && xs4<=xs2 && xs4<=xs3) Pos_Agv[Num].x1=xs4;

    if(xs1>=xs2 && xs1>=xs3 && xs1>=xs4) Pos_Agv[Num].x2=xs1;
    if(xs2>=xs1 && xs2>=xs3 && xs2>=xs4) Pos_Agv[Num].x2=xs2;
    if(xs3>=xs1 && xs3>=xs2 && xs3>=xs4) Pos_Agv[Num].x2=xs3;
    if(xs4>=xs1 && xs4>=xs2 && xs4>=xs3) Pos_Agv[Num].x2=xs4;

    if(ys1<=ys2 && ys1<=ys3 && ys1<=ys4) Pos_Agv[Num].y1=ys1;
    if(ys2<=ys1 && ys2<=ys3 && ys2<=ys4) Pos_Agv[Num].y1=ys2;
    if(ys3<=ys1 && ys3<=ys2 && ys3<=ys4) Pos_Agv[Num].y1=ys3;
    if(ys4<=ys1 && ys4<=ys2 && ys4<=ys3) Pos_Agv[Num].y1=ys4;

    if(ys1>=ys2 && ys1>=ys3 && ys1>=ys4) Pos_Agv[Num].y2=ys1;
    if(ys2>=ys1 && ys2>=ys3 && ys2>=ys4) Pos_Agv[Num].y2=ys2;
    if(ys3>=ys1 && ys3>=ys2 && ys3>=ys4) Pos_Agv[Num].y2=ys3;
    if(ys4>=ys1 && ys4>=ys2 && ys4>=ys3) Pos_Agv[Num].y2=ys4;

    Pos_Agv[Num].x1=(Pos_Agv[Num].x1/Rap*Rid);
    Pos_Agv[Num].x2=(Pos_Agv[Num].x2/Rap*Rid);
    Pos_Agv[Num].y1=(Pos_Agv[Num].y1/Rap*Rid);
    Pos_Agv[Num].y2=(Pos_Agv[Num].y2/Rap*Rid);

    myDist=Pos_Agv[Num].x2-Pos_Agv[Num].x1;
    myDist=myDist/4;
    Pos_Agv[Num].x1=Pos_Agv[Num].x1+myDist;
    Pos_Agv[Num].x2=Pos_Agv[Num].x2-myDist;

    myDist=Pos_Agv[Num].y2-Pos_Agv[Num].y1;
    myDist=myDist/4;
    Pos_Agv[Num].y1=Pos_Agv[Num].y1+myDist;
    Pos_Agv[Num].y2=Pos_Agv[Num].y2-myDist;

//  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RRRRRRRRRRRRRRRRRRRRRR

  //-------------------------------------------
  // Solo disegno TGV se nessun box a bordo
  //-------------------------------------------
  StlBOX = bsSolid;
  // Se ANNOVI in DEBUG visualizza Trasparente
  if(VisualizzaZoneImpianto || VisualizzaTgvTrasparente[Num]){
     StlBOX     = bsClear;
     ColPLOGICO = ColTGV;
  }

  //-------------------------------------------
  // Disegna numero TGV solo su angolo retto
  //-------------------------------------------
  if(Rid>=RID_NUMERI && (grd==0 || grd==90 || grd==180 || grd==270)){
     xnum = (xcen-30)/Rap*Rid;
     ynum = (ycen-30)/Rap*Rid;
     sprintf(num_agv, "%02d", Num);
     Sfondo->Canvas->Font->Color=clSFONDO;
     if(VisualizzaZoneImpianto || VisualizzaTgvTrasparente[Num]) Sfondo->Canvas->Font->Color=ColTGV;
     Sfondo->Canvas->TextOut(xnum, ynum, num_agv);
     Sfondo->Canvas->Font->Color=clSFONDO;
  }

  //------------------------------------------
  // DISEGNA PALETTA BORDO
  // Dimensioni e colore in base al tipo box
  // e al prodotto contenuto
  //------------------------------------------
  if(carA || carB){
     //-------------------------------------------------------
     // Recupero le dimensioni del pallet dalla struttura
     // TIPIBOX
     //-------------------------------------------------------
     if(AGV[Num]->mission.PresCEST_B!=LIBERA){    // FORCHE "B"
        lung_cest        = M.TipiBox[AGV[Num]->mission.BoxB.TipoBox].Lung;
        larg_cest        = M.TipiBox[AGV[Num]->mission.BoxB.TipoBox].Larg;
        dist_tra_palette = M.TipiBox[AGV[Num]->mission.BoxB.TipoBox].dist_box_in_magaz;
     }
     else{                                        // FORCHE "A"
        lung_cest        = M.TipiBox[AGV[Num]->mission.BoxA.TipoBox].Lung;
        larg_cest        = M.TipiBox[AGV[Num]->mission.BoxA.TipoBox].Larg;
        dist_tra_palette = M.TipiBox[AGV[Num]->mission.BoxA.TipoBox].dist_box_in_magaz;
     }
     //-------------------------------------------------------
     // Calcolo formato grafico
     //-------------------------------------------------------
     mezzo_lung = lung_cest/2;
     mezzo_larg = larg_cest/2;
     //-------------------------------------------------------
     // Disegno di ogni box previsto in presa
     //-------------------------------------------------------
     for(i=0; i<POSINPRESA; i++){
        for(j=0; j<PIANIINPRESA; j++){
           //-------------------------------------------------------
           // OffSet di posizionamento PALETTA
           //-------------------------------------------------------
           offsX=offsY=0;
           offsY = (LUNG_AREA_CARICO/2) - mezzo_lung;
         //offsY = ((LUNG_AREA_CARICO/2) - mezzo_lung) - (LUNG_AREA_CARICO/2) - (dist_tra_palette/2);
           //-------------------------------------------------------
           // Incremento offset posizione TGV da nodo
           //-------------------------------------------------------
           offsY = offsY + (i * (lung_cest+dist_tra_palette));
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (PIANIINPRESA-j))/2) - mezzo_larg) - (j*(mezzo_larg));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);
           //-------------------------------------------------------
           // Costruzione del pallet attorno alle coordinate di
           // centro pallet stesso
           //-------------------------------------------------------
           MezzoLatoX = (long double)(mezzo_larg);
           MezzoLatoY = (long double)(mezzo_lung);

           radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           radB1 = radA + radB;
           radB2 = radA - radB;

           //-------------------------------------------------------
           // Per una corretta visualizzazione in modalità BOBINE
           // il quadrato su cui viene costruita, non può essere
           // inclinato come la navetta ma deve essere sempre
           // ortogonale allo schermo per evitare deformazioni
           //-------------------------------------------------------
           switch(tipo_carico){
              case  0: radB1 = radA + radB;       // rettangolo
                       radB2 = radA - radB;
                       break;
              case  1: radB1 =    0 + radB;       // ellisse
                       radB2 =    0 - radB;
                       break;
              default: radB1 = radA + radB;       // rettangolo
                       radB2 = radA - radB;
                       break;
           }

           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

           xs1 = (int) (xpal-(int)(ComponenteX1));
           xs2 = (int) (xpal+(int)(ComponenteX2));
           xs3 = (int) (xpal+(int)(ComponenteX1));
           xs4 = (int) (xpal-(int)(ComponenteX2));

           ys1 = (int) (ypal+(int)(ComponenteY1));
           ys2 = (int) (ypal-(int)(ComponenteY2));
           ys3 = (int) (ypal-(int)(ComponenteY1));
           ys4 = (int) (ypal+(int)(ComponenteY2));
           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xs4-xs3);
              if(offs1<3) xs4=xs3;
              offs1 = abs(xs2-xs1);
              if(offs1<3) xs2=xs1;
              offs1 = abs(ys4-ys1);
              if(offs1<3) ys4=ys1;
              offs1 = abs(ys3-ys2);
              if(offs1<3) ys3=ys2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xs4-xs1);
              if(offs1<3) xs4=xs1;
              offs1 = abs(xs3-xs2);
              if(offs1<3) xs3=xs2;
              offs1 = abs(ys2-ys1);
              if(offs1<3) ys2=ys1;
              offs1 = abs(ys4-ys3);
              if(offs1<3) ys4=ys3;
           }

           // --- disegno riquadro pallet con Forca Singola
           points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
           points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
           points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
           points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

           // --- disegno riquadro pallet con Doppia Forca
           pointA[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
           pointA[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
           pointA[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);

           pointB[0] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
           pointB[1] = Point(xs4/Rap*Rid, ys4/Rap*Rid);
           pointB[2] = Point(xs1/Rap*Rid, ys1/Rap*Rid);

           //-------------------------------------------
           // Disegno PALLET A BORDO
           //-------------------------------------------
         //StlBOX = bsSolid;
           StlBOX = bsDiagCross;
           // Se ANNOVI in DEBUG visualizza Trasparente
           if(VisualizzaZoneImpianto || VisualizzaTgvTrasparente[Num]) StlBOX = bsClear;
           //--------------------------------------------
           // GESTIONE DOPPIA FORCA
           // Visualizzazione presenza pallet su doppia
           // forca
           //--------------------------------------------
           if(GestioneDoppiaForcaTGV!=0){
              //--------------------------------------------
              // Visualizzazione in base al tipo di carico
              //--------------------------------------------
              switch(tipo_carico){
                 // RETTANGOLO
                 case  0: // FORCA "B"
                          Sfondo->Canvas->Pen->Color = ColPEN_B;
                          if(AGV[Num]->mission.BoxB.Presa[i][j]==true) Sfondo->Canvas->Brush->Color = ColBOX_B;
                          else                                         Sfondo->Canvas->Brush->Color = clSFONDO;
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          Sfondo->Canvas->Polygon(pointB, 2);
                          // FORCA "A"
                          Sfondo->Canvas->Pen->Color = ColPEN_A;
                          if(AGV[Num]->mission.BoxA.Presa[i][j]==true) Sfondo->Canvas->Brush->Color = ColBOX_A;
                          else                                         Sfondo->Canvas->Brush->Color = clSFONDO;
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          Sfondo->Canvas->Polygon(pointA, 2);
                          break;
                 // ELLISSE
                 case  1: //
                          // visualizzazione da studiare quando sarà necessario
                          //
                          break;
                 // RETTANGOLO (default)
                 default: // FORCA "B"
                          Sfondo->Canvas->Pen->Color = ColPEN_B;
                          if(AGV[Num]->mission.BoxB.Presa[i][j]==true) Sfondo->Canvas->Brush->Color = ColBOX_B;
                          else                                         Sfondo->Canvas->Brush->Color = clSFONDO;
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          Sfondo->Canvas->Polygon(pointB, 2);
                          // FORCA "A"
                          Sfondo->Canvas->Pen->Color = ColPEN_A;
                          if(AGV[Num]->mission.BoxA.Presa[i][j]==true) Sfondo->Canvas->Brush->Color = ColBOX_A;
                          else                                         Sfondo->Canvas->Brush->Color = clSFONDO;
                          Sfondo->Canvas->Brush->Style = StlBOX;
                          Sfondo->Canvas->Polygon(pointA, 2);
                          break;
                 }                          
           }
           //--------------------------------------------
           // GESTIONE FORCA SINGOLA
           // Visualizzazione presenza pallet singola
           //--------------------------------------------
           else{
              if(AGV[Num]->mission.BoxB.Presa[i][j]==true){
                 Sfondo->Canvas->Pen->Color   = ColPEN_B;
                 Sfondo->Canvas->Brush->Color = ColBOX_B;
                 Sfondo->Canvas->Brush->Style = StlBOX;
                 //--------------------------------------------
                 // Visualizzazione in base al tipo di carico
                 //--------------------------------------------
                 switch(tipo_carico){
                    case  0: Sfondo->Canvas->Polygon(points, 3);    // rettangolo
                             break;
                    case  1: Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             break;
                    default: Sfondo->Canvas->Polygon(points, 3);    // rettangolo
                             break;
                 }
              }
           }
        }
     }
  }

//Sfondo->Canvas->Pen->Color   = clFuchsia;           // sfondo NERO per il Pallet
//Sfondo->Canvas->Polyline(lines, 1);

  //-------------------------------------------
  // Disegno Centro PUNTO LOGICO
  //-------------------------------------------
  Sfondo->Canvas->Pen->Color = ColPLOGICO;
  xx1 = ((Xl1-Xl2)/2+Xl2);
  yy1 = ((Yl1-Yl2)/2+Yl2);
  xx2 = ((Xl3-Xl4)/2+Xl4);
  yy2 = ((Yl3-Yl4)/2+Yl4);

  xx1 = (xx1-xlog)/4+xlog;
  yy1 = (yy1-ylog)/4+ylog;
  xx2 = (xx2-xlog)/4+xlog;
  yy2 = (yy2-ylog)/4+ylog;


  Sfondo->Canvas->MoveTo( xx1/Rap*Rid, yy1/Rap*Rid );
  Sfondo->Canvas->LineTo( xx2/Rap*Rid, yy2/Rap*Rid );

  xx1 = ((Xl2-Xl3)/2+Xl3);
  yy1 = ((Yl2-Yl3)/2+Yl3);
  xx2 = ((Xl1-Xl4)/2+Xl4);
  yy2 = ((Yl1-Yl4)/2+Yl4);

  xx1 = (xx1-xlog)/4+xlog;
  yy1 = (yy1-ylog)/4+ylog;
  xx2 = (xx2-xlog)/4+xlog;
  yy2 = (yy2-ylog)/4+ylog;

  Sfondo->Canvas->MoveTo( xx1/Rap*Rid, yy1/Rap*Rid );
  Sfondo->Canvas->LineTo( xx2/Rap*Rid, yy2/Rap*Rid );

  // Se ANNOVI in DEBUG visualizza Trasparente
  if(VisualizzaTgvTrasparente[Num]){
     Sfondo->Canvas->Pen->Color   = clSFONDO;
     Sfondo->Canvas->MoveTo( Xl3/Rap*Rid, Yl3/Rap*Rid );
     Sfondo->Canvas->LineTo( Xl4/Rap*Rid, Yl4/Rap*Rid );
  }

  //-------------------------------------------
  // Con TGV IN ALLARME e NON IN RISERVA
  // lampeggia di rosso la cornice esterna
  //-------------------------------------------
  if((AGV[Num]->stato.s.bit.allarme && !AGV[Num]->stato.s.bit.ris) || AGV[Num]->allarme_interno){
     //-----------------------------------------
     // contorno la superficie di tutto il TGV
     //-----------------------------------------
     larg_all = larg_agv;
     lung_all = lung_agv;
     if(larg_agv<LARG_AREA_CARICO) larg_all = LARG_AREA_CARICO;
     if(lung_agv<LUNG_AREA_CARICO) lung_all = LUNG_AREA_CARICO;
     //-----------------------------------------
     // Allarme lampeggiante
     //-----------------------------------------
     if(!Lampeggio){
        Sfondo->Canvas->Pen->Color   = clRed;
        Sfondo->Canvas->Brush->Color = clRed;
        Sfondo->Canvas->Brush->Style = bsClear;

        radA = DegToRad(grd);

        MezzoLatoX = (long double)((larg_all+20)/2);
        MezzoLatoY = (long double)((lung_all+20)/2);

        radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
        radB1 = radA + radB;
        radB2 = radA - radB;
        SinCos(radB1, sinB1, cosB1);
        SinCos(radB2, sinB2, cosB2);

        MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

        ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
        ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
        ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
        ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

        xs1 = (int) (xcen-(int)(ComponenteX1));
        xs2 = (int) (xcen+(int)(ComponenteX2));
        xs3 = (int) (xcen+(int)(ComponenteX1));
        xs4 = (int) (xcen-(int)(ComponenteX2));

        ys1 = (int) (ycen+(int)(ComponenteY1));
        ys2 = (int) (ycen-(int)(ComponenteY2));
        ys3 = (int) (ycen-(int)(ComponenteY1));
        ys4 = (int) (ycen+(int)(ComponenteY2));

        //-----------------------------------------------
        // Aggiustamenti dovuti ad arrotondamenti
        //-----------------------------------------------
        if(grd==90 || grd==270){
           offs1 = abs(xs4-xs3);
           if(offs1<3) xs4=xs3;
           offs1 = abs(xs2-xs1);
           if(offs1<3) xs2=xs1;
           offs1 = abs(ys4-ys1);
           if(offs1<3) ys4=ys1;
           offs1 = abs(ys3-ys2);
           if(offs1<3) ys3=ys2;
        }
        if(grd==0 || grd==180){
           offs1 = abs(xs4-xs1);
           if(offs1<3) xs4=xs1;
           offs1 = abs(xs3-xs2);
           if(offs1<3) xs3=xs2;
           offs1 = abs(ys2-ys1);
           if(offs1<3) ys2=ys1;
           offs1 = abs(ys4-ys3);
           if(offs1<3) ys4=ys3;
        }

        //-----------------------------------------------
        // Disegno poligono di allarme
        //-----------------------------------------------
        points[0] = Point(xs1/Rap*Rid, ys1/Rap*Rid);
        points[1] = Point(xs2/Rap*Rid, ys2/Rap*Rid);
        points[2] = Point(xs3/Rap*Rid, ys3/Rap*Rid);
        points[3] = Point(xs4/Rap*Rid, ys4/Rap*Rid);

        Sfondo->Canvas->Polygon(points, 3);
     }
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       DisegnaBox()
// ----------------------------------------------
// In base alle presenze a bordo TGV
//
void TFormLayout::DisegnaBox(short int PresCEST_A, short int PresCEST_B, TColor *ColBOX_A, TColor *ColBOX_B)
/***************************************/
{
  int  i;
  TColor  ColA = clSFONDO;
  TColor  ColB = clSFONDO;

  //------------------------------------------------------
  // Colore in base alla presenza a bordo con priorità
  // alle prensenza sulla forca alta
  //------------------------------------------------------
  // FORCHE BASSE
  switch(PresCEST_B){
     case  LIBERA:        ColB = (TColor)(clSFONDO);
                          break;
     case  ST_BOX_LAV1:   ColB = (TColor)(COLOR_PRES_STAT1);      // default (verde)
                          break;
     case  ST_BOX_LAV2:   ColB = (TColor)(COLOR_PRES_STAT2);      // default (verde)
                          break;
     case  ST_BOX_LAV3:   ColB = (TColor)(COLOR_PRES_STAT3);      // default (verde)
                          break;
     case  ST_BOX_LAV4:   ColB = (TColor)(COLOR_PRES_STAT4);      // default (verde)
                          break;
     case  ST_BOX_LAV5:   ColB = (TColor)(COLOR_PRES_STAT5);      // default (rosso)
                          break;
     case  ST_BOX_LAV6:   ColB = (TColor)(COLOR_PRES_STAT6);      // default (rosso)
                          break;
     case  ST_BOX_LAV7:   ColB = (TColor)(COLOR_PRES_STAT7);      // default (rosso)
                          break;
     case  ST_BOX_LAV8:   ColB = (TColor)(COLOR_PRES_STAT8);      // default (rosso)
                          break;
     case  ST_BOX_VUOTI:  ColB = (TColor)(COLOR_PRES_VUOTI);     // default (bianco)
                          break;
  }
  // FORCHE ALTE
  switch(PresCEST_A){
     case  LIBERA:        ColA = (TColor)(clSFONDO);
                          break;
     case  ST_BOX_LAV1:   ColA = (TColor)(COLOR_PRES_STAT1);      // default (verde)
                          break;
     case  ST_BOX_LAV2:   ColA = (TColor)(COLOR_PRES_STAT2);      // default (verde)
                          break;
     case  ST_BOX_LAV3:   ColA = (TColor)(COLOR_PRES_STAT3);      // default (verde)
                          break;
     case  ST_BOX_LAV4:   ColA = (TColor)(COLOR_PRES_STAT4);      // default (verde)
                          break;
     case  ST_BOX_LAV5:   ColA = (TColor)(COLOR_PRES_STAT5);      // default (rosso)
                          break;
     case  ST_BOX_LAV6:   ColA = (TColor)(COLOR_PRES_STAT6);      // default (rosso)
                          break;
     case  ST_BOX_LAV7:   ColA = (TColor)(COLOR_PRES_STAT7);      // default (rosso)
                          break;
     case  ST_BOX_LAV8:   ColA = (TColor)(COLOR_PRES_STAT8);      // default (rosso)
                          break;
     case  ST_BOX_VUOTI:  ColA = (TColor)(COLOR_PRES_VUOTI);     // default (bianco)
                          break;
  }
  //------------------------------------------------------
  // Attribuzione dello stile e del Colore al Box
  //------------------------------------------------------
  *ColBOX_A=ColA;
  *ColBOX_B=ColB;
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       DisegnaRiflettoriLaser()
// ----------------------------------------------
// Num = numero AGV (1..MAXAGV)
// Diseggna graficamente i riflettori in campo per il TGV
// per ogni singola Mappa.
//
void TFormLayout::DisegnaRiflettoriLaser( int Num )
/***************************************/
{
  int Riflettori, Mappa;
  int xcen,ycen;
  int X1, Y1, X2, Y2;
  char buffer[11]="";
  TColor Col;
  int RaggioZona;

  //-------------------------------------------------------
  // Scrolla tutte le zone Mapping del TGV
  //-------------------------------------------------------
  if( LaserMappaRiflettoriTgv==Num ){
     for( Mappa=0; Mappa<MAXZONEMAPPING; Mappa++){
        if( AGV[ Num ]->zone_mapping.Z[Mappa].ZAbiltata==0 )  continue;
        if( AGV[ Num ]->zone_mapping.Z[Mappa].Nr==0 )         continue;
        if( LaserMappaRiflettoriZona!=Mappa && LaserMappaRiflettoriZona!=99 ) continue;
        //---------------------------------------------------
        // Selezione col in base a mappa in visualizzazione
        //---------------------------------------------------
        switch(Mappa){
           case  1: Col = clWhite;
                    break;
           case  2: Col = clAqua;
                    break;
           case  3: Col = clLime;
                    break;
           case  4: Col = clRed;
                    break;
           case  5: Col = clOlive;
                    break;
           case  6: Col = clFuchsia;
                    break;
           case  7: Col = clTeal;
                    break;
           case  8: Col = (TColor)(0x00FFA4FF);
                    break;
           case  9: Col = (TColor)(0x0088AEFF);
                    break;
           case 10: Col = (TColor)(0x0080FFFF);
                    break;
           default: Col = clYellow;
                    break;
        }
        //---------------------------------------------------
        // Disegna anche il centro zona
        //---------------------------------------------------
        Sfondo->Canvas->Brush->Style = bsSolid;
        Sfondo->Canvas->Pen->Color   = Col;
        Sfondo->Canvas->Brush->Color = Col;

        xcen =  AGV[ Num ]->zone_mapping.Z[Mappa].Xl/10;
        ycen = -AGV[ Num ]->zone_mapping.Z[Mappa].Yl/10;
        xcen= xcen-offset[1][0];      // pixel x del nodo occupato
        ycen= ycen-offset[1][1];      // pixel y del nodo occupato
        xcen = xcen/Rap*Rid;
        ycen = ycen/Rap*Rid;

        X1 = xcen-6;
        Y1 = ycen-6;
        X2 = xcen+6;
        Y2 = ycen+6;

        Sfondo->Canvas->Brush->Style = bsClear ;

        Sfondo->Canvas->Ellipse( X1, Y1, X2, Y2);
//      sprintf( buffer, "%d", Mappa );
//      StampNum( Sfondo, X2 , Y1-1, buffer , Col );
        sprintf(buffer, "Z%02d", Mappa);
        Sfondo->Canvas->Font->Color = Col;
        Sfondo->Canvas->TextOut(X2, Y1-10, buffer);

        X1 = xcen-3;
        Y1 = ycen-3;
        X2 = xcen+3;
        Y2 = ycen+3;

        Sfondo->Canvas->Pen->Color   = clBlue;
        Sfondo->Canvas->Brush->Color = clBlue;
        Sfondo->Canvas->Ellipse( X1, Y1, X2, Y2);

        RaggioZona = AGV[ Num ]->zone_mapping.Z[Mappa].Rl/10*1000/Rap*Rid;
        X1 = xcen-RaggioZona;
        Y1 = ycen-RaggioZona;
        X2 = xcen+RaggioZona;
        Y2 = ycen+RaggioZona;
        Sfondo->Canvas->Brush->Style = bsClear ;
        Sfondo->Canvas->Pen->Color   = Col;
        Sfondo->Canvas->Ellipse( X1, Y1, X2, Y2);
        //---------------------------------------------------
        // Disegna i Riflettori (sempre gialli)
        //---------------------------------------------------
        Sfondo->Canvas->Brush->Style = bsSolid;
        Sfondo->Canvas->Pen->Color   = Col;
        Sfondo->Canvas->Brush->Color = Col;
        Col = clYellow;
        for( Riflettori=0; Riflettori<MAXRIFLETTORI; Riflettori++){
           if( Riflettori >= AGV[ Num ]->zone_mapping.Z[Mappa].Nr ) break;
           xcen =  AGV[ Num ]->zone_mapping.Z[Mappa].Xr[ Riflettori ]/10;
           ycen = -AGV[ Num ]->zone_mapping.Z[Mappa].Yr[ Riflettori ]/10;
           xcen= xcen-offset[1][0];      // pixel x del nodo occupato
           ycen= ycen-offset[1][1];      // pixel y del nodo occupato
           xcen = xcen/Rap*Rid;
           ycen = ycen/Rap*Rid;

           X1 = xcen-3;
           Y1 = ycen-3;
           X2 = xcen+3;
           Y2 = ycen+3;

           Sfondo->Canvas->Ellipse( X1, Y1, X2, Y2);
           sprintf( buffer, "%d", Riflettori );
           StampNum( Sfondo, X2 , Y1-1, buffer , Col, TRUE, 0);
        }
     }
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_porte()
// ----------------------------------------------
void TFormLayout::disegna_porte()
/********************************************/
{

  int i;
  int x1, y1, x2, y2, xs, ys;
  char sigla[11]="";

  Sfondo->Canvas->Brush->Style = bsSolid;
  Sfondo->Canvas->Brush->Color = clSFONDO;
  Sfondo->Canvas->Pen->Color   = clSFONDO;

  for(i=1; i<=TOTPORTE; i++){                // per tutte le celle
     if(Porte[i][1]==0 &&
        Porte[i][2]==0 &&
        Porte[i][3]==0 &&
        Porte[i][4]==0 &&
        Porte[i][5]==0) break;
     //----------------------------------------------------------------------------
     // Coordinate per la visualizzazione della sigla
     //----------------------------------------------------------------------------
     x1=y1=x2=y2=xs=ys=0;
     if(Porte[i][0]!=0) xs=(int)(((Porte[i][0]-offset[1][0])/Rap)*Rid);
     if(Porte[i][1]!=0) ys=(int)(((Porte[i][1]-offset[1][1])/Rap)*Rid);
     if(Porte[i][2]!=0) x1=(int)(((Porte[i][2]-offset[1][0])/Rap)*Rid);
     if(Porte[i][3]!=0) y1=(int)(((Porte[i][3]-offset[1][1])/Rap)*Rid);
     if(Porte[i][4]!=0) x2=(int)(((Porte[i][4]-offset[1][0])/Rap)*Rid);
     if(Porte[i][5]!=0) y2=(int)(((Porte[i][5]-offset[1][1])/Rap)*Rid);
     //----------------------------------------------------------------------------
     // Selezione colore di visualizzazione
     //----------------------------------------------------------------------------
     Sfondo->Canvas->Font->Color = (TColor)(0x008080FF);
     Sfondo->Canvas->Pen->Color  = (TColor)(0x008080FF);
     // TGV IN ARRIVO
     if(PorteImp[i].TgvInArrivo==true){
        Sfondo->Canvas->Font->Color = (TColor)(0x009BFFFF);
        Sfondo->Canvas->Pen->Color  = (TColor)(0x009BFFFF);
     }
     // APERTURA TOTALE
     if(PorteImp[i].OkIngresso==true && PorteImp[i].AperturaParziale==false){
        Sfondo->Canvas->Font->Color = clLime;
        if(Lampeggio) Sfondo->Canvas->Pen->Color  = clLime;
     }
     // APERTURA PARZIALE
     if(PorteImp[i].OkIngresso==true && PorteImp[i].AperturaParziale==true){
        Sfondo->Canvas->Font->Color = (TColor)(0x0080FF80);
        if(Lampeggio) Sfondo->Canvas->Pen->Color  = (TColor)(0x0080FF80);
     }
     // PORTA IN BLOCCO
     if(PorteImp[i].PortaInBlocco){
        Sfondo->Canvas->Font->Color = clSFONDO;
        Sfondo->Canvas->Pen->Color  = (TColor)(0x00FF8080);
        if(Lampeggio) Sfondo->Canvas->Font->Color = (TColor)(0x00FF8080);
     }
     // PORTA ESCLUSA DA PLC
     if(PorteImp[i].EsclusionePLC){
        Sfondo->Canvas->Font->Color = clSFONDO;
        Sfondo->Canvas->Pen->Color  = clAqua;
        if(Lampeggio) Sfondo->Canvas->Font->Color = clAqua;
     }
     //----------------------------------------------------------------------------
     // Disegno della porta
     //----------------------------------------------------------------------------
     if(!PorteImp[i].OkIngresso || Lampeggio){
        Sfondo->Canvas->MoveTo( x1, y1 );
        Sfondo->Canvas->LineTo( x2, y2 );
        //----------------------------------------------------------------------------
        // Ingrossamento della porta
        //----------------------------------------------------------------------------
        if(x1==x2){
           Sfondo->Canvas->MoveTo( x1+1, y1 );
           Sfondo->Canvas->LineTo( x2+1, y2 );
        }
        if(y1==y2){
           Sfondo->Canvas->MoveTo( x1, y1+1 );
           Sfondo->Canvas->LineTo( x2, y2+1 );
        }
     }
     //----------------------------------------------------------------------------
     // Compilazione della sigla porta
     //----------------------------------------------------------------------------
     if(Rid>=RID_NUMERI && xs!=0 && ys!=0){
        sprintf(sigla, "P%02d", i);
        Sfondo->Canvas->TextOut(xs, ys, sigla);
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_macchine()
// ----------------------------------------------
void TFormLayout::disegna_macchine()
/********************************************/
{
  char buff[31]="";
  char sigla[31]="";
  int i, j, s, z;
  bool lamp_esterno;
  int  lamp_interno;
  short int TipoPresA, TipoPresB;
  TColor ColBA, ColBB, ColP;
  int xs, ys, xcen, xpal, ycen, ypal, offs1;
  int xinc1, xinc2, xinc3, xinc4;
  int yinc1, yinc2, yinc3, yinc4;
  int sviluppo, nodo, tipo, posti, grd;
  int larg_cest        = LARG_CEST;
  int lung_cest        = LUNG_CEST;
  int dist_tra_palette = DIST_CEST_IN_MAG;
  int carico_in_punta  = CARICO_IN_PUNTA;
  int tipo_carico      = TIPO_CARICO_TGV;
  long double offsY_nod, offsX_nod, offsY, offsX, hip;
  long double sinA, cosA, sinB, cosB, sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double radA_m, radA, radB, radC, radB1, radB2;
  long double radB1_r, radB2_r, sinB1_r, cosB1_r, sinB2_r, cosB2_r;
  long double ComponenteXOffset, ComponenteYOffset;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  TPoint pointA[4];
  TPoint pointB[4];
  TPoint points[5];
  TPoint points_r[5];

  //-------------------------------------------
  // Disegno del Palettizzatore come insieme
  // di Stazioni
  //-------------------------------------------
  for(i=1; i<=TOTCHIAMATE; i++){                // per tutte le macchine
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(P.ch[i]->Num<1           ) continue;
     if(P.ch[i]->Num>TOTCHIAMATE ) continue;
     //-------------------------------------------------------
     // Recupero dati legati alla singola macchina
     //-------------------------------------------------------
     sviluppo = Macch[i][0];                    // Direzione di sviluppo (direzione da
                                                // primo cestone depositato -> a nodo)
     tipo     = P.ch[i]->Tipo;                  // Tipo macchina
     //-------------------------------------------------------
     // Coordinate per la visualizzazione della sigla
     //-------------------------------------------------------
     xs=ys=0;
     if(Macch[i][3]!=0) xs=(int)(((Macch[i][3]-offset[1][0])/Rap)*Rid);
     if(Macch[i][4]!=0) ys=(int)(((Macch[i][4]-offset[1][1])/Rap)*Rid);
     //-------------------------------------------------------
     // Colori di default
     //-------------------------------------------------------
     ColBA = clSFONDO;
     ColBB = clSFONDO;
     ColP  = clSFONDO;
     //-------------------------------------------------------
     // Ciclo per tutte le stazioni
     //-------------------------------------------------------
     for(s=0; s<MAXSTAZMAC; s++){                // per tutte le stazioni
        //-------------------------------------------------------
        // dati
        //-------------------------------------------------------
        nodo     = P.ch[i]->Staz[s].PuntoIng;     // Numero del nodo di imgresso magazzino
        posti    = P.ch[i]->Staz[s].Npt;          // Posti previsti su stazione
        if(nodo==0) continue;                     // Se il Nodo del Magazzino=0 ---> Skip!
        //-------------------------------------------------------
        // Locazione inesistente  (posti=0)
        //-------------------------------------------------------
        if(posti==0  ) continue;

        //-------------------------------------------------------
        // 30/04/97 Disegna con l'offset relativo alla pagina
        //-------------------------------------------------------
        xcen = ((Punti[nodo][0]-offset[1][0]));            // pixel x del nodo occupato
        ycen = ((Punti[nodo][1]-offset[1][1]));            // pixel y del nodo occupato

        //-------------------------------------------------------
        // Recupero i gradi di rotazione in base allo sviluppo
        // assegnato
        //-------------------------------------------------------
        grd = sviluppo;
        //-------------------------------------------------------
        // Aggiustamento dello sviluppo standard ai gradi
        //-------------------------------------------------------
        if(sviluppo==0) grd=0;
        if(sviluppo==1) grd=180;
        if(sviluppo==2) grd=90;
        if(sviluppo==3) grd=270;
        //-------------------------------------------------------
        // Gradi to Radianti
        //-------------------------------------------------------
        radA_m = DegToRad(grd);
        //-------------------------------------------------------
        // Calcolo delle dimensioni grafiche della paletta
        //-------------------------------------------------------
        MezzoLatoX = (long double)(larg_cest/2);
        MezzoLatoY = (long double)(lung_cest/2);
        //-------------------------------------------------------
        // Inizilizzo offSet di posizionamento PALETTA
        //-------------------------------------------------------
        offsX_nod = 0;
        offsY_nod = 0;  // inizializzazione con offset manuale
        if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
        else{
           offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
        }
        //-------------------------------------------------------
        // Disegno di tutte le postazioni
        //-------------------------------------------------------
        for(j=1; j<=posti; j++){
           //-------------------------------------------------------
           // Incremento offset posizione TGV da nodo
           //-------------------------------------------------------
           offsX = offsX_nod;
           offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette));
           //-------------------------------------------------------
           // Recupero gradi singola stazione
           //-------------------------------------------------------
           radA = radA_m;
           if(P.ch[i]->Staz[s].RotLayout!=0){
              radA = DegToRad(P.ch[i]->Staz[s].RotLayout);
           }
           //-------------------------------------------------------
           // Aggiornamento presenza in magazzino
           //-------------------------------------------------------
           TipoPresA = P.ch[i]->Staz[s].Pos[j-1].PresCEST_A;
           TipoPresB = P.ch[i]->Staz[s].Pos[j-1].PresCEST_B;
           //-------------------------------------------------------
           // Se sulla stazione non c'è presenza, ma c'è chiamata
           // visualizzo il lampeggio con il colore del carro
           // richiesto
           //-------------------------------------------------------
           if(TipoPresB==LIBERA && P.ch[i]->Staz[s].Richiesta==true) TipoPresB = P.ch[i]->Staz[s].StatoLav;
           //--------------------------------------
           // Visualizzazione tanti riquadri
           // quante le tot posizini in presa
           //--------------------------------------
           for(z=0; z<Macch[i][2]; z++){
              //-------------------------------------------------------
              // Calcolo coordinate grafiche del centro paletta
              //-------------------------------------------------------
              offsX = (((larg_cest * (Macch[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
              //-------------------------------------------------------
              // ipotenusa del triangolo rettangolo i cui lati sono
              // gli offset del centro paletta dal centro navetta
              //-------------------------------------------------------
              radB  = (long double)(atanl( offsX / offsY ));
              SinCos(radB, sinB, cosB);
              hip   = offsY / cosB;
              //-------------------------------------------------------
              // Calcolo nell'angolo di rotazione del centro paletta
              // come sommatoria dei gradi del TGV + l'angolo di
              // rotazione dal centro TGV al centro paletta
              //-------------------------------------------------------
              radC = radA + radB;
              SinCos(radC, sinC, cosC);
              //-------------------------------------------------------
              // Offset in X,Y del centro PALLET rispetto al centro TGV
              //-------------------------------------------------------
              ComponenteYOffset = hip * cosC;
              ComponenteXOffset = hip * sinC;

              xpal = xcen-(int)(ComponenteXOffset);
              ypal = ycen-(int)(ComponenteYOffset);
              //-------------------------------------------------------
              // Recupero coordinate vertici box per visualizzazione
              //-------------------------------------------------------
              radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

              //-------------------------------------------------------
              // Per una corretta visualizzazione in modalità BOBINE
              // il quadrato su cui viene costruita, non può essere
              // inclinato come la navetta ma deve essere sempre
              // ortogonale allo schermo per evitare deformazioni
              //-------------------------------------------------------
              switch(tipo_carico){
                 case  0: radB1 = radA + radB;       // rettangolo
                          radB2 = radA - radB;
                          break;
                 case  1: radB1 =    0 + radB;       // ellisse
                          radB2 =    0 - radB;
                          //---------------------------------------
                          // calcolo comunque le coordinate
                          // del rettangolo per il contorno
                          //---------------------------------------
                          radB1_r = radA + radB;     // rettangolo
                          radB2_r = radA - radB;
                          SinCos(radB1_r, sinB1_r, cosB1_r);
                          SinCos(radB2_r, sinB2_r, cosB2_r);

                          MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

                          ComponenteX1 = (long double) ( MezzaDiagonale*cosB1_r);
                          ComponenteX2 = (long double) ( MezzaDiagonale*cosB2_r);
                          ComponenteY1 = (long double) ( MezzaDiagonale*sinB1_r);
                          ComponenteY2 = (long double) ( MezzaDiagonale*sinB2_r);

                          xinc1 = (int) (xpal-(int)(ComponenteX1));
                          xinc2 = (int) (xpal+(int)(ComponenteX2));
                          xinc3 = (int) (xpal+(int)(ComponenteX1));
                          xinc4 = (int) (xpal-(int)(ComponenteX2));

                          yinc1 = (int) (ypal+(int)(ComponenteY1));
                          yinc2 = (int) (ypal-(int)(ComponenteY2));
                          yinc3 = (int) (ypal-(int)(ComponenteY1));
                          yinc4 = (int) (ypal+(int)(ComponenteY2));

                          //-----------------------------------------------
                          // Aggiustamenti dovuti ad arrotondamenti
                          //-----------------------------------------------
                          if(grd==90 || grd==270){
                             offs1 = abs(xinc4-xinc3);
                             if(offs1<3) xinc4=xinc3;
                             offs1 = abs(xinc2-xinc1);
                             if(offs1<3) xinc2=xinc1;
                             offs1 = abs(yinc4-yinc1);
                             if(offs1<3) yinc4=yinc1;
                             offs1 = abs(yinc3-yinc2);
                             if(offs1<3) yinc3=yinc2;
                          }
                          if(grd==0 || grd==180){
                             offs1 = abs(xinc4-xinc1);
                             if(offs1<3) xinc4=xinc1;
                             offs1 = abs(xinc3-xinc2);
                             if(offs1<3) xinc3=xinc2;
                             offs1 = abs(yinc2-yinc1);
                             if(offs1<3) yinc2=yinc1;
                             offs1 = abs(yinc4-yinc3);
                             if(offs1<3) yinc4=yinc3;
                          }

                          //-----------------------------------------------
                          // Disegno poligono postazione singola
                          //-----------------------------------------------
                          points_r[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
                          points_r[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
                          points_r[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
                          points_r[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);
                          break;
                 default: radB1 = radA + radB;       // rettangolo
                          radB2 = radA - radB;
                          break;
              }

              SinCos(radB1, sinB1, cosB1);
              SinCos(radB2, sinB2, cosB2);

              MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

              ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
              ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
              ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
              ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

              xinc1 = (int) (xpal-(int)(ComponenteX1));
              xinc2 = (int) (xpal+(int)(ComponenteX2));
              xinc3 = (int) (xpal+(int)(ComponenteX1));
              xinc4 = (int) (xpal-(int)(ComponenteX2));

              yinc1 = (int) (ypal+(int)(ComponenteY1));
              yinc2 = (int) (ypal-(int)(ComponenteY2));
              yinc3 = (int) (ypal-(int)(ComponenteY1));
              yinc4 = (int) (ypal+(int)(ComponenteY2));

              //-----------------------------------------------
              // Aggiustamenti dovuti ad arrotondamenti
              //-----------------------------------------------
              if(grd==90 || grd==270){
                 offs1 = abs(xinc4-xinc3);
                 if(offs1<3) xinc4=xinc3;
                 offs1 = abs(xinc2-xinc1);
                 if(offs1<3) xinc2=xinc1;
                 offs1 = abs(yinc4-yinc1);
                 if(offs1<3) yinc4=yinc1;
                 offs1 = abs(yinc3-yinc2);
                 if(offs1<3) yinc3=yinc2;
              }
              if(grd==0 || grd==180){
                 offs1 = abs(xinc4-xinc1);
                 if(offs1<3) xinc4=xinc1;
                 offs1 = abs(xinc3-xinc2);
                 if(offs1<3) xinc3=xinc2;
                 offs1 = abs(yinc2-yinc1);
                 if(offs1<3) yinc2=yinc1;
                 offs1 = abs(yinc4-yinc3);
                 if(offs1<3) yinc4=yinc3;
              }

              //-----------------------------------------------
              // Disegno poligono postazione singola
              //-----------------------------------------------
              points[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
              points[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
              points[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
              points[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);

              pointA[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
              pointA[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
              pointA[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);

              pointB[0] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
              pointB[1] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);
              pointB[2] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);

              //-----------------------------------------------
              // Scelta colori contorno
              //-----------------------------------------------
              ColP = clLINEE;
              ColBB= clSFONDO;
              ColBA= clSFONDO;
              //-----------------------------------------------
              // Colori contorno
              // ESCLUSIONE       = fuchsia
              //-----------------------------------------------
              if(tipo==MC_NONE                                            ) ColP=clFuchsia;
              if(P.ch[i]->Esclusione || P.ch[i]->Staz[s].Esclusione==true ) ColP=clFuchsia;
              //-----------------------------------------------
              // Visualizzazioni riservata a stati baie e non
              // scaffali
              //-----------------------------------------------
              if(P.ch[i]->Blocco                 && !Lampeggio) ColP=clRed;
              if(!P.ch[i]->Automatico            && !Lampeggio) ColP=clYellow;
              if(P.ch[i]->EsclComunic            && !Lampeggio) ColP=clPurple;
              if(P.ch[i]->FineProd               && !Lampeggio) ColP=clTeal;
              if(P.ch[i]->Staz[s].NoMagaz==true  && !Lampeggio) ColP=(TColor)(0x000080FF);
              if(P.ch[i]->Staz[s].NoCodice==true && !Lampeggio) ColP=clAqua;
              //-----------------------------------------------
              // Recupero colori interni
              //-----------------------------------------------
              if(TipoPresA!=LIBERA || TipoPresB!=LIBERA){
                 DisegnaBox(TipoPresA, TipoPresB, &ColBA, &ColBB);
              }
              //-----------------------------------------------
              // Rettangolo nero per baia libera o lampeggio
              // chiamata
              //-----------------------------------------------
              if(((j==1 && P.ch[i]->Staz[s].Richiesta==true) || (j==posti && P.ch[i]->Staz[s].Espulsione==true)) && !Lampeggio){
                 Sfondo->Canvas->Pen->Color   = ColP;          // sfondo NERO per il Pallet
                 //-----------------------------------------------
                 // Lampeggio in base al tipo della baia
                 // PRELIEVO: lampeggio nero.
                 // DEPOSITO: lampeggio con il colore del tipo
                 //           pallet da portare
                 //-----------------------------------------------
               //  if(P.ch[i]->Staz[s].Espulsione==true) Sfondo->Canvas->Brush->Color = clSFONDO;
               //  if(P.ch[i]->Staz[s].Richiesta==true ) Sfondo->Canvas->Brush->Color = ColBB;
                 //-----------------------------------------------
                 // Lampeggio colore nero
                 //-----------------------------------------------
                 Sfondo->Canvas->Brush->Color = clSFONDO;
                 //-----------------------------------------------
                 // visualizzazione trasparente
                 //-----------------------------------------------
                 Sfondo->Canvas->Brush->Style = bsSolid;
                 if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                 //-----------------------------------------------
                 // Rettangolo
                 //-----------------------------------------------
                 Sfondo->Canvas->Polygon(points, 3);
              }
              else{
                 //--------------------------------------------
                 // GESTIONE DOPPIA FORCA
                 // Visualizzazione presenza pallet su doppia
                 // forca
                 //--------------------------------------------
                 if(GestioneDoppiaForcaTGV==1){
                    //--------------------------------------------
                    // Visualizzazione in base al tipo di carico
                    //--------------------------------------------
                    switch(tipo_carico){
                       case  0: // RETTANGOLO (PALLET)
                                // POSIZIONE BASSA
                                Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                                Sfondo->Canvas->Brush->Color = ColBB;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                // triangolo posizone BASSA
                                Sfondo->Canvas->Polygon(pointB, 2);
                                // POSIZIONE ALTA
                                Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX ALTO
                                Sfondo->Canvas->Brush->Color = ColBA;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                // triangolo posizone ALTA
                                Sfondo->Canvas->Polygon(pointA, 2);
                                break;
                       case  1: // ELLISSE (BOBINA)
                                Sfondo->Canvas->Pen->Color   = ColP;
                                Sfondo->Canvas->Brush->Color = clSFONDO;
                                Sfondo->Canvas->Polygon(points_r, 3);       // rettangolo esterno
                                if(TipoPresA!=LIBERA || TipoPresB!=LIBERA){
                                   Sfondo->Canvas->Pen->Color   = ColBB;  // ellisse interna (BOBINA)
                                   Sfondo->Canvas->Brush->Color = ColBB;
                                   Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                                }
                                break;
                       default: // RETTANGOLO (PALLET)
                                // POSIZIONE BASSA
                                Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                                Sfondo->Canvas->Brush->Color = ColBB;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                // triangolo posizone BASSA
                                Sfondo->Canvas->Polygon(pointB, 2);
                                // POSIZIONE ALTA
                                Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX ALTO
                                Sfondo->Canvas->Brush->Color = ColBA;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                // triangolo posizone ALTA
                                Sfondo->Canvas->Polygon(pointA, 2);
                                break;
                    }
                 }
                 //--------------------------------------------
                 // GESTIONE FORCA SINGOLA
                 // Visualizzazione presenza pallet singola
                 //--------------------------------------------
                 else{
                    //--------------------------------------------
                    // Visualizzazione in base al tipo di carico
                    //--------------------------------------------
                    switch(tipo_carico){
                       case  0: // RETTANGOLO (PALLET)
                                // SOLO POSIZIONE BASSA
                                Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                                if(P.ba[i]->Presa[j-1][z][0]==true) Sfondo->Canvas->Brush->Color = ColBB;
                                else                                Sfondo->Canvas->Brush->Color = clSFONDO;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                Sfondo->Canvas->Polygon(points, 3);       // rettangolo
                                break;
                       case  1: // ELLISSE (BOBINA)
                                // SOLO POSIZIONE BASSA
                                Sfondo->Canvas->Pen->Color   = ColP;
                                Sfondo->Canvas->Brush->Color = clSFONDO;
                                Sfondo->Canvas->Polygon(points_r, 3);       // rettangolo esterno
                                if(TipoPresA!=LIBERA || TipoPresB!=LIBERA){
                                   Sfondo->Canvas->Pen->Color   = ColBB;  // ellisse interna (BOBINA)
                                   Sfondo->Canvas->Brush->Color = ColBB;
                                   Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                                }
                                break;
                       default: // RETTANGOLO (PALLET)
                                // SOLO POSIZIONE BASSA
                                Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                                if(P.ba[i]->Presa[j-1][z][0]==true) Sfondo->Canvas->Brush->Color = ColBB;
                                else                                Sfondo->Canvas->Brush->Color = clSFONDO;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                Sfondo->Canvas->Polygon(points, 3);       // rettangolo
                                break;
                    }
                 }
              }
           }
        }
     }
     //-------------------------------------------------------
     // Scrittura SIGLA MACCHINA
     //-------------------------------------------------------
     if(Rid>=RID_NUMERI){
        sprintf(sigla, "%02d", i);
        if(strcmp(P.ch[i]->Sigla, "")!=0){
           sprintf(buff, " - %s", P.ch[i]->Sigla);   // sigla della macchina
           strcat(sigla, buff);
        }
        Sfondo->Canvas->Brush->Color = clSFONDO;
        Sfondo->Canvas->Brush->Style = bsSolid;
        Sfondo->Canvas->Font->Color  = clAqua;                               // Automatico
        //---------------------------------------------
        // Colorazione font in base a stato macchina
        //---------------------------------------------
        if(!P.ch[i]->Automatico) Sfondo->Canvas->Font->Color = clYellow;     // Manuale
        if(P.ch[i]->Blocco     ) Sfondo->Canvas->Font->Color = clRed;        // Blocco/Allarme
        if(P.ch[i]->Esclusione ) Sfondo->Canvas->Font->Color = clFuchsia;    // Esclusione
        //---------------------------------------------
        // Scrittura sigla su sfondo grafico
        //---------------------------------------------
        if(xs!=0 || ys!=0) Sfondo->Canvas->TextOut(xs, ys, sigla);
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_cbatt()
// ----------------------------------------------
void TFormLayout::disegna_cbatt()
{
  int i;
  int xcen,ycen;
  int xs, ys;
  int xinc1, xinc2, xinc3, xinc4;
  int yinc1, yinc2, yinc3, yinc4;
  int num_nod;
  short int gruppo;
  int larg_cest,lung_cest;
  char sigla[21]="";
  long double offsY, offsX;
  long double MezzoLatoX, MezzoLatoY;
  TPoint points[5];
  TColor ColP;
  //-------------------------------------------------------
  // Default visualizzo le posizione previste da grafica
  //-------------------------------------------------------
  larg_cest = LARG_CEST;
  lung_cest = LUNG_CEST;
  MezzoLatoX = (long double)(larg_cest/2);
  MezzoLatoY = (long double)(lung_cest/2);
  //-------------------------------------------------------
  // Disegno del Palettizzatore come insieme di stazioni
  //-------------------------------------------------------
  for(i=0; i<=TOTBATT; i++){  // per tutti i cambiobatterie
     //-------------------------------------------------------
     // condizioni di skip
     //-------------------------------------------------------
     if(P.cb[i]->Num<1       ) continue;
     if(P.cb[i]->Num>TOTBATT ) continue;
     if(P.cb[i]->Punto==0    ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     num_nod = P.cb[i]->Punto;  // Numero del nodo
     gruppo  = P.cb[i]->Gruppo;
     //-------------------------------------------------------
     // Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[num_nod][0]-offset[1][0])); // pixel x del nodo
     ycen = ((Punti[num_nod][1]-offset[1][1])); // pixel y del nodo
     //-------------------------------------------------------
     // Inizializzo offset di posizionamento
     //-------------------------------------------------------
     offsX = CBattGraph[i][0];  // offset manuale X
     offsY = CBattGraph[i][1];  // offset manuale Y
     //-----------------------------------------------
     // Punti dei vertici del poligono
     //-----------------------------------------------
     xinc1 = xcen - MezzoLatoX + offsX;    yinc1 = ycen - MezzoLatoY + offsY;
     xinc2 = xcen + MezzoLatoX + offsX;    yinc2 = ycen - MezzoLatoY + offsY;
     xinc3 = xcen + MezzoLatoX + offsX;    yinc3 = ycen + MezzoLatoY + offsY;
     xinc4 = xcen - MezzoLatoX + offsX;    yinc4 = ycen + MezzoLatoY + offsY;
     //-----------------------------------------------
     // Disegno poligono
     //-----------------------------------------------
     points[0] = Point(xinc1 / Rap * Rid, yinc1 / Rap * Rid);
     points[1] = Point(xinc2 / Rap * Rid, yinc2 / Rap * Rid);
     points[2] = Point(xinc3 / Rap * Rid, yinc3 / Rap * Rid);
     points[3] = Point(xinc4 / Rap * Rid, yinc4 / Rap * Rid);
     //-----------------------------------------------
     // Colori contorno
     //-----------------------------------------------
     ColP = clLINEE;
     //-----------------------------------------------
     // Recupero il colore dalla Gruppo
     //-----------------------------------------------
     if(gruppo!=NN && P.GruppiBaie[gruppo].Col!=0) ColP=P.GruppiBaie[gruppo].Col;
     //-----------------------------------------------
     // Stato CAMBIO BATTERIA
     //-----------------------------------------------
     if(!P.cb[i]->Automatico  && !Lampeggio) ColP = clYellow;
     if( P.cb[i]->Blocco      && !Lampeggio) ColP = clRed;
     if( P.cb[i]->Esclusione  && !Lampeggio) ColP = clFuchsia;
     //-----------------------------------------------
     // Colora cambio batteria
     //-----------------------------------------------
     Sfondo->Canvas->Brush->Color = ColP;// clWhite;
     Sfondo->Canvas->Brush->Style = bsDiagCross;
     Sfondo->Canvas->Pen->Color   = ColP;
     Sfondo->Canvas->Polygon(points, 3);
     //------------------------------------------
     // Visualizzazione SIGLA del cambiobatterie
     //------------------------------------------
     if( Rid>=RID_NUMERI ){
        //-------------------------------------------------------
        // Inizializzo offSet di posizionamento
        //-------------------------------------------------------
        offsX = CBattGraph[i][2];  // offset manuale X Sigla
        offsY = CBattGraph[i][3];  // offset manuale Y Sigla
        //--------------------------------------------------------
        // Determinato le coordinate per la Sigla
        //--------------------------------------------------------
        xs=ys=0;
        if(offsX!=0 && offsY !=0){
           xs = (offsX-offset[1][0]) / Rap * Rid;
           ys = (offsY-offset[1][1]) / Rap * Rid;
        }
        //------------------------------------------
        // Scrittura SIGLA
        //------------------------------------------
        strcpy(sigla, P.cb[i]->Sigla);
        Sfondo->Canvas->Font->Color  = clWhite;
        Sfondo->Canvas->Brush->Color = clSFONDO;
        Sfondo->Canvas->Brush->Style = bsSolid;
        //---------------------------------------------
        // Scrittura sigla su sfondo grafico
        //---------------------------------------------
        if(xs!=0 || ys!=0) Sfondo->Canvas->TextOut(xs, ys, sigla);
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_baie()
// ----------------------------------------------
void TFormLayout::disegna_baie()
/********************************************/
{
  int i, j, z;
  char num[5]="";
  bool PresA, PresB;
  bool lamp_esterno;
  bool pos_riservata;
  bool tgv_su_baia;
  short int IDA, IDB, gruppo;
  TColor ColBA, ColBB, ColP;
  int xcen, xpal, ycen, ypal, offs1;
  int xinc1, xinc2, xinc3, xinc4;
  int yinc1, yinc2, yinc3, yinc4;
//int x_num, y_num;
  int sviluppo, nodo, tipo, posti, grd;
  int larg_cest = LARG_CEST;
  int lung_cest = LUNG_CEST;
  int dist_tra_palette = DIST_CEST_IN_MAG;
  int carico_in_punta  = CARICO_IN_PUNTA;
  int tipo_carico      = TIPO_CARICO_TGV;
  short int num_mac1, TipoPresA, TipoPresB, num_mag1;
  long double offsY_nod, offsX_nod, offsY, offsX, hip;
  long double sinA, cosA, sinB, cosB, sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double radA, radB, radC, radB1, radB2;
  long double radB1_r, radB2_r, sinB1_r, cosB1_r, sinB2_r, cosB2_r;
  long double ComponenteXOffset, ComponenteYOffset;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  TPoint points[5];
  TPoint points_r[5];
  TPoint pointA[4];
  TPoint pointB[4];

  //-------------------------------------------
  // Disegno del Palettizzatore come insieme
  // di Stazioni
  //-------------------------------------------
  for(i=1; i<=MAXBAIE; i++){                // per tutte le baie
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(P.ba[i]->Num<1       ) continue;
     if(P.ba[i]->Num>TOTBAIE ) continue;
     //-------------------------------------------------------
     // Colori di default
     //-------------------------------------------------------
     ColBA = clSFONDO;
     ColBB = clSFONDO;
     ColP  = clSFONDO;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     sprintf(num, "%d", P.ba[i]->Num); // Numero della baia
     nodo       = P.ba[i]->Punto;      // Numero del nodo di imgresso magazzino
     tipo       = P.ba[i]->Tipo;       // Tipo di locazione
     gruppo     = P.ba[i]->Gruppo;     // Gruppo
     sviluppo   = Baie[i][0];          // Direzione di sviluppo (direzione da
                                       // primo cestone depositato -> a nodo)
     posti      = Baie[i][1];          // Massimo numero cestoni
     if(nodo==0) continue;             // Se il Nodo del Magazzino=0 ---> Skip!
     //-------------------------------------------------------
     // Locazione inesistente  (posti=0)
     //-------------------------------------------------------
     if(posti==0) continue;
     //----------------------------------------------------------
     // Verifico se c'è un TGV sul nodo della baia con missione
     // in corso sulla baia per automatizzare l'esplosione
     //----------------------------------------------------------
     tgv_su_baia = false;
     for(j=1; j<=MAXAGV; j++){
        if(AGV[j]->stato.pos!=nodo) continue;
        tgv_su_baia=true;
        break;
     }

     //-------------------------------------------------------
     // Numero baia
     //-------------------------------------------------------
     sprintf(num, "%d", P.ba[i]->Num);

     //-------------------------------------------------------
     // 30/04/97 Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[nodo][0]-offset[1][0]));            // pixel x del nodo occupato
     ycen = ((Punti[nodo][1]-offset[1][1]));            // pixel y del nodo occupato

     //-------------------------------------------------------
     // Recupero i gradi di rotazione in base allo sviluppo
     // assegnato
     //-------------------------------------------------------
     grd = sviluppo;
     //-------------------------------------------------------
     // Aggiustamento dello sviluppo standard ai gradi
     //-------------------------------------------------------
     if(sviluppo==0) grd=0;
     if(sviluppo==1) grd=180;
     if(sviluppo==2) grd=90;
     if(sviluppo==3) grd=270;
     //-------------------------------------------------------
     // Gradi to Radianti
     //-------------------------------------------------------
     radA = DegToRad(grd);
     //-------------------------------------------------------
     // Calcolo delle dimensioni grafiche della paletta
     //-------------------------------------------------------
     MezzoLatoX = (long double)(larg_cest/2);
     MezzoLatoY = (long double)(lung_cest/2);
     //-------------------------------------------------------
     // Inizilizzo offSet di posizionamento PALETTA
     //-------------------------------------------------------
     offsX_nod = 0;
     offsY_nod = Baie[i][3];  // inizializzazione con offset manuale
     if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
     else{
        offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
     }
     //-------------------------------------------------------
     // Disegno di tutte le postazioni
     //-------------------------------------------------------
     for(j=1; j<=posti; j++){
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsX = offsX_nod;
        offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette));
        //-------------------------------------------------------
        // Aggiornamento presenza in magazzino
        //-------------------------------------------------------
        PresA     = false;
        PresB     = false;
        TipoPresA = P.ba[i]->TipoCod[j-1];
        TipoPresB = P.ba[i]->TipoCod[j-1];
        //-------------------------------------------------------
        // Sulla prima poszione si fa riferimento alla struttura
        // Box
        //-------------------------------------------------------
        if(j==1){
           TipoPresA = P.ba[i]->PresCEST_A;
           TipoPresB = P.ba[i]->PresCEST_B;
        }
        if(TipoPresA!=LIBERA) PresA = true;
        if(TipoPresB!=LIBERA) PresB = true;
        //--------------------------------------
        // Visualizzazione tanti riquadri
        // quante le tot posizini in presa
        //--------------------------------------
        for(z=0; z<Baie[i][2]; z++){
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (Baie[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);

           //-------------------------------------------------------
           // Recupero coordinate vertici box per visualizzazione
           //-------------------------------------------------------
           radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           //-------------------------------------------------------
           // Per una corretta visualizzazione in modalità BOBINE
           // il quadrato su cui viene costruita, non può essere
           // inclinato come la navetta ma deve essere sempre
           // ortogonale allo schermo per evitare deformazioni
           //-------------------------------------------------------
           switch(tipo_carico){
              case  0: radB1 = radA + radB;       // rettangolo
                       radB2 = radA - radB;
                       break;
              case  1: radB1 =    0 + radB;       // ellisse
                       radB2 =    0 - radB;
                       //---------------------------------------
                       // calcolo comunque le coordinate
                       // del rettangolo per il contorno
                       //---------------------------------------
                       radB1_r = radA + radB;     // rettangolo
                       radB2_r = radA - radB;
                       SinCos(radB1_r, sinB1_r, cosB1_r);
                       SinCos(radB2_r, sinB2_r, cosB2_r);

                       MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

                       ComponenteX1 = (long double) ( MezzaDiagonale*cosB1_r);
                       ComponenteX2 = (long double) ( MezzaDiagonale*cosB2_r);
                       ComponenteY1 = (long double) ( MezzaDiagonale*sinB1_r);
                       ComponenteY2 = (long double) ( MezzaDiagonale*sinB2_r);

                       xinc1 = (int) (xpal-(int)(ComponenteX1));
                       xinc2 = (int) (xpal+(int)(ComponenteX2));
                       xinc3 = (int) (xpal+(int)(ComponenteX1));
                       xinc4 = (int) (xpal-(int)(ComponenteX2));

                       yinc1 = (int) (ypal+(int)(ComponenteY1));
                       yinc2 = (int) (ypal-(int)(ComponenteY2));
                       yinc3 = (int) (ypal-(int)(ComponenteY1));
                       yinc4 = (int) (ypal+(int)(ComponenteY2));

                       //-----------------------------------------------
                       // Aggiustamenti dovuti ad arrotondamenti
                       //-----------------------------------------------
                       if(grd==90 || grd==270){
                          offs1 = abs(xinc4-xinc3);
                          if(offs1<3) xinc4=xinc3;
                          offs1 = abs(xinc2-xinc1);
                          if(offs1<3) xinc2=xinc1;
                          offs1 = abs(yinc4-yinc1);
                          if(offs1<3) yinc4=yinc1;
                          offs1 = abs(yinc3-yinc2);
                          if(offs1<3) yinc3=yinc2;
                       }
                       if(grd==0 || grd==180){
                          offs1 = abs(xinc4-xinc1);
                          if(offs1<3) xinc4=xinc1;
                          offs1 = abs(xinc3-xinc2);
                          if(offs1<3) xinc3=xinc2;
                          offs1 = abs(yinc2-yinc1);
                          if(offs1<3) yinc2=yinc1;
                          offs1 = abs(yinc4-yinc3);
                          if(offs1<3) yinc4=yinc3;
                       }

                       //-----------------------------------------------
                       // Disegno poligono postazione singola
                       //-----------------------------------------------
                       points_r[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
                       points_r[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
                       points_r[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
                       points_r[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);
                       break;
              default: radB1 = radA + radB;       // rettangolo
                       radB2 = radA - radB;
                       break;
           }

           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

           xinc1 = (int) (xpal-(int)(ComponenteX1));
           xinc2 = (int) (xpal+(int)(ComponenteX2));
           xinc3 = (int) (xpal+(int)(ComponenteX1));
           xinc4 = (int) (xpal-(int)(ComponenteX2));

           yinc1 = (int) (ypal+(int)(ComponenteY1));
           yinc2 = (int) (ypal-(int)(ComponenteY2));
           yinc3 = (int) (ypal-(int)(ComponenteY1));
           yinc4 = (int) (ypal+(int)(ComponenteY2));

           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xinc4-xinc3);
              if(offs1<3) xinc4=xinc3;
              offs1 = abs(xinc2-xinc1);
              if(offs1<3) xinc2=xinc1;
              offs1 = abs(yinc4-yinc1);
              if(offs1<3) yinc4=yinc1;
              offs1 = abs(yinc3-yinc2);
              if(offs1<3) yinc3=yinc2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xinc4-xinc1);
              if(offs1<3) xinc4=xinc1;
              offs1 = abs(xinc3-xinc2);
              if(offs1<3) xinc3=xinc2;
              offs1 = abs(yinc2-yinc1);
              if(offs1<3) yinc2=yinc1;
              offs1 = abs(yinc4-yinc3);
              if(offs1<3) yinc4=yinc3;
           }

           //-----------------------------------------------
           // Disegno poligono postazione singola
           //-----------------------------------------------
           points[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
           points[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
           points[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
           points[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);

           pointA[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
           pointA[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
           pointA[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);

           pointB[0] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
           pointB[1] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);
           pointB[2] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
           //-----------------------------------------------
           // Scelta colori contorno
           //-----------------------------------------------
           ColP = clLINEE;
           //-----------------------------------------------
           // Recupero il colore dalla Gruppo
           //-----------------------------------------------
           if(gruppo!=NN && P.GruppiBaie[gruppo].Col!=0) ColP=P.GruppiBaie[gruppo].Col;
           //-----------------------------------------------
           // Colori contorno
           // ESCLUSIONE       = fuchsia
           //-----------------------------------------------
           if(tipo==BA_NONE                     ) ColP=clFuchsia;
           if(P.ba[i]->Esclusione  && !Lampeggio) ColP=clFuchsia;
           //-----------------------------------------------
           // Visusalizzazioni riservata a stati baie e non
           // scaffali
           //-----------------------------------------------
           if(P.ba[i]->NoMagaz     && !Lampeggio) ColP=clAqua;
           if(P.ba[i]->Blocco      && !Lampeggio) ColP=clRed;
           if(!P.ba[i]->Automatico && !Lampeggio) ColP=clYellow;
           //-----------------------------------------------
           // Recupero colori interni
           //-----------------------------------------------
           if(PresA || PresB){
              DisegnaBox(TipoPresA, TipoPresB, &ColBA, &ColBB);
           }
           //-----------------------------------------------
           // Rettangolo nero per baia libera o lampeggio
           // chiamata
           //-----------------------------------------------
           if(j==1 && P.ba[i]->Chiamata==true && !Lampeggio){
              Sfondo->Canvas->Pen->Color   = ColP;          // sfondo NERO per il Pallet
              //-----------------------------------------------
              // Lampeggio in base al tipo della baia
              // PRELIEVO: lampeggio nero.
              // DEPOSITO: lampeggio con il colore del tipo
              //           pallet da portare
              //-----------------------------------------------
              if(P.ba[i]->Tipo==BA_PRELIEVO) Sfondo->Canvas->Brush->Color = clSFONDO;
              if(P.ba[i]->Tipo==BA_DEPOSITO) Sfondo->Canvas->Brush->Color = ColBB;
           /*
              //-----------------------------------------------
              // visualizzazione trasparente
              //-----------------------------------------------
              Sfondo->Canvas->Brush->Style = bsSolid;
              if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
           */
              //-----------------------------------------------
              // visualizzazione trasparente
              //-----------------------------------------------
              Sfondo->Canvas->Brush->Style = bsSolid;
              if(VisualizzaZoneImpianto || (P.ba[i]->StatoPres==NN && tgv_su_baia==true)) Sfondo->Canvas->Brush->Style = bsClear;
              //-----------------------------------------------
              // GESTIONE ANOMALIE
              // Per visualizzare comunque lo stato di chiamata
              // anche se non c'è presenza fisica scelgo la
              // griglia bianca
              //-----------------------------------------------
              if(PresA==false && PresB==false && P.ba[i]->StatoPres==NN && tgv_su_baia==false && VisualizzaZoneImpianto==false){
                 Sfondo->Canvas->Brush->Color = clWhite;
                 Sfondo->Canvas->Brush->Style = bsDiagCross;
              }
              //-----------------------------------------------
              // Rettangolo
              //-----------------------------------------------
              Sfondo->Canvas->Polygon(points, 3);
           }
           //-----------------------------------------------
           // Triangolo colorato per ogni posizione
           //------------------------------------------
           else{
              //--------------------------------------------
              // GESTIONE DOPPIA FORCA
              // Visualizzazione presenza pallet su doppia
              // forca
              //
              // Per prima posizione baie se presenza alta
              // e bassa contemporaneamente o su scaffale
              // se ho una solo paletta
              //--------------------------------------------
              if(j==1 && PresA && PresB && (GestioneDoppiaForcaTGV==1 || GestioneDoppiaForcaTGV==2)){
                 //--------------------------------------------
                 // Visualizzazione in base al tipo di carico
                 //--------------------------------------------
                 switch(tipo_carico){
                    case  0: // RETTANGOLO (PALLET)
                             // POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                             Sfondo->Canvas->Brush->Color = ColBB;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone BASSA
                             Sfondo->Canvas->Polygon(pointB, 2);
                             // POSIZIONE ALTA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX ALTO
                             Sfondo->Canvas->Brush->Color = ColBA;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone ALTA
                             Sfondo->Canvas->Polygon(pointA, 2);
                             break;
                    case  1: // ELLISSE (BOBINA)
                             Sfondo->Canvas->Pen->Color   = ColP;
                             Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             Sfondo->Canvas->Polygon(points_r, 3);     // rettangolo esterno
                             if(P.ba[i]->Presa[j-1][z][0]==true){
                                Sfondo->Canvas->Pen->Color   = ColBB;  // ellisse interna (BOBINA)
                                Sfondo->Canvas->Brush->Color = ColBB;
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             }
                             break;
                    default: // RETTANGOLO (PALLET)
                             // POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                             Sfondo->Canvas->Brush->Color = ColBB;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone BASSA
                             Sfondo->Canvas->Polygon(pointB, 2);
                             // POSIZIONE ALTA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX ALTO
                             Sfondo->Canvas->Brush->Color = ColBA;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone ALTA
                             Sfondo->Canvas->Polygon(pointA, 2);
                             break;
                 }
              }
              //--------------------------------------------
              // GESTIONE FORCA SINGOLA
              // Visualizzazione presenza pallet singola
              //--------------------------------------------
              else{
                 //--------------------------------------------
                 // Visualizzazione in base al tipo di carico
                 //--------------------------------------------
                 switch(tipo_carico){
                    case  0: // RETTANGOLO (PALLET)
                             // SOLO POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                             if(P.ba[i]->Presa[j-1][z][0]==true) Sfondo->Canvas->Brush->Color = ColBB;
                             else                                Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto || (tgv_su_baia==true && PresB==false)) Sfondo->Canvas->Brush->Style = bsClear;
                             Sfondo->Canvas->Polygon(points, 3);       // rettangolo
                             break;
                    case  1: // ELLISSE (BOBINA)
                             // SOLO POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;
                             Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto || (tgv_su_baia==true && PresB==false)) Sfondo->Canvas->Brush->Style = bsClear;
                             Sfondo->Canvas->Polygon(points_r, 3);       // rettangolo esterno
                             if(P.ba[i]->Presa[j-1][z][0]==true){
                                Sfondo->Canvas->Pen->Color   = ColBB;  // ellisse interna (BOBINA)
                                Sfondo->Canvas->Brush->Color = ColBB;
                                if(VisualizzaZoneImpianto || (tgv_su_baia==true && PresB==false)) Sfondo->Canvas->Brush->Style = bsClear;
                                Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             }
                             break;
                    default: // RETTANGOLO (PALLET)
                             // SOLO POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                             if(P.ba[i]->Presa[j-1][z][0]==true) Sfondo->Canvas->Brush->Color = ColBB;
                             else                                Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(VisualizzaZoneImpianto || (tgv_su_baia==true && PresB==false)) Sfondo->Canvas->Brush->Style = bsClear;
                             Sfondo->Canvas->Polygon(points, 3);       // rettangolo
                             break;
                 }
                 //------------------------------------
                 // Stampa pallet alto basso
                 //------------------------------------
                 if( Rid>=RID_NUMERI && tipo_carico!=1 && P.ba[i]->TipoCod[j-1]!=ST_BOX_VUOTI ){
                    //---------------------------------------------------
                    // Se è presente un pallet al sovrapposto in presa
                    // inserisco un "2" nella visualizzazione
                    //
                    // sostituita visualizzazione alti bassi con CODICE
                    //---------------------------------------------------
                    strcpy(num, "");
                    if(P.ba[i]->Presa[j-1][z][0]==true) sprintf(num, "%02d", P.ba[i]->Codice[j-1]);
                    StampaStringa( Sfondo, ((xpal-20)/Rap*Rid), ((ypal-4)/Rap*Rid), num, clBlack);
                 }
              }
           }
        }
     }
     //------------------------------------------
     // Visualizzazione del numero del magazzino
     //------------------------------------------
     if( Rid>=RID_NUMERI ){
     // StampNum(FormLayout->Sfondo, x_num, y_num, num, clNUMLOC, FALSE, Punti[i][3]);
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_magazzini()
// ----------------------------------------------
void TFormLayout::disegna_magazzini()
/********************************************/
{
  int  i, j, k, z;
  char num[5]="";
  char sigla[21]="";
  bool PresA, PresB;
  bool tgv_su_fila;
  bool lamp_esterno;
  bool pos_riservata;
//short int IDA, IDB;
  TColor ColBA, ColBB, ColP;
  int xcen, ycen, xpal, ypal;
  int xs, ys, offs1;
  int xinc1, xinc2, xinc3, xinc4;
  int yinc1, yinc2, yinc3, yinc4;
  int xnum, ynum, offnum;
  int sviluppo, nodo, nodo_p, tipo, posti, grd;
  int larg_cest = LARG_CEST;
  int lung_cest = LUNG_CEST;
  int PianiInPresa     = PIANIINPRESA;
  int dist_tra_palette = DIST_CEST_IN_MAG;
  int carico_in_punta  = CARICO_IN_PUNTA;
  int tipo_carico      = TIPO_CARICO_TGV;
  short int abilita_piani, offset_da_nodo, GrpDestA, GrpDestB, gruppo;
  short int TipoPresA, TipoPresB, TotPal, TotPalA, TotPalB, tipo_box;
  long double diff, offsX_nod, offsY_nod, offsY, offsX, hip;
  long double sinA, cosA, sinB, cosB, sinC, cosC, sinB1, cosB1, sinB2, cosB2;
  long double radA, radB, radC, radB1, radB2;
  long double ComponenteXOffset, ComponenteYOffset;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  long double MezzoLatoX1, MezzoLatoY1, MezzaDiagonale1;
  TColor clSigla;
  TPoint points[5];
  TPoint pointA[4];
  TPoint pointB[4];
  char idbox[5]="";

  //-------------------------------------------
  // Disegno del Palettizzatore come insieme
  // di Stazioni
  //-------------------------------------------
  for(i=1; i<=TOTMAG; i++){                // per tutti i magazzini
     //-------------------------------------------------------
     // Se la macchina non esiste --> skip!!
     //-------------------------------------------------------
     if(M.m[i]->Num<1      ) continue;
     if(M.m[i]->Num>TOTMAG ) continue;
     //-------------------------------------------------------
     // dati
     //-------------------------------------------------------
     nodo       = M.m[i]->PuntoIng;   // Numero del nodo di imgresso magazzino
     tipo       = M.m[i]->Tipo;       // Tipo di locazione
     tipo_box   = M.m[i]->TipoBox;    // Tipo di Box
     gruppo     = M.m[i]->Gruppo;
     sviluppo   = Magaz[i][0];        // Direzione di sviluppo (direzione da
                                      // primo cestone depositato -> a nodo)
     posti      = M.m[i]->Npt;        // Capacità magazzino
     //-------------------------------------------------------
     // Nodo errato
     //-------------------------------------------------------
     if(nodo<1              ) continue;
     if(nodo>TOTPUNTI       ) continue;
     //-------------------------------------------------------
     // Verifico se TGV su fila
     //-------------------------------------------------------
     tgv_su_fila=false;
     if(N.nodo_busy[nodo]>0 && N.nodo_busy[nodo]<=MAXAGV) tgv_su_fila=true;
     //-------------------------------------------------------
     // Locazione inesistente  (posti=0)
     //-------------------------------------------------------
     if(posti==0) continue;
     //-------------------------------------------------------
     // Magazzino abilitato alla sovrapposizione
     //-------------------------------------------------------
     abilita_piani = M.m[i]->Sovrapposizione;
     //-------------------------------------------------------
     // Coordinate per la visualizzazione della sigla
     //-------------------------------------------------------
     xs=ys=0;
     if(Magaz[i][3]!=0) xs=(int)(((Magaz[i][3]-offset[1][0])/Rap)*Rid);
     if(Magaz[i][4]!=0) ys=(int)(((Magaz[i][4]-offset[1][1])/Rap)*Rid);
     //-------------------------------------------------------
     // Numero magazzino
     //-------------------------------------------------------
     sprintf(num, "%02d", M.m[i]->Num);
     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     lung_cest         = M.TipiBox[tipo_box].Lung;
     larg_cest         = M.TipiBox[tipo_box].Larg;
     dist_tra_palette  = M.TipiBox[tipo_box].dist_box_in_magaz;
     offset_da_nodo    = M.m[i]->OffSetDaNodo + M.TipiBox[tipo_box].offset_tgv_da_nodo_mag;
     //-------------------------------------------------------
     // 30/04/97 Disegna con l'offset relativo alla pagina
     //-------------------------------------------------------
     xcen = ((Punti[nodo][0]-offset[1][0]));            // pixel x del nodo occupato
     ycen = ((Punti[nodo][1]-offset[1][1]));            // pixel y del nodo occupato

     //-------------------------------------------------------
     // Recupero i gradi di rotazione in base allo sviluppo
     // assegnato
     //-------------------------------------------------------
     grd = sviluppo;
     //-------------------------------------------------------
     // Aggiustamento dello sviluppo standard ai gradi
     //-------------------------------------------------------
     if(sviluppo==0) grd=0;
     if(sviluppo==1) grd=180;
     if(sviluppo==2) grd=90;
     if(sviluppo==3) grd=270;
     //-------------------------------------------------------
     // (TEST DELLE ROTAZIONI)
     // Recupero gradi da edit di test rotazioni
     //-------------------------------------------------------
     if(EditGrd->Visible==true){
        grd = atof(EditGrd->Text.c_str());
     }
     //-------------------------------------------------------
     // Gradi to Radianti
     //-------------------------------------------------------
     radA = DegToRad(grd);
     //-------------------------------------------------------
     // Calcolo delle dimensioni grafiche della paletta
     //-------------------------------------------------------
     MezzoLatoX = (long double)(larg_cest/2);
     MezzoLatoY = (long double)(lung_cest/2);
     //-------------------------------------------------------
     // Inizilizzo offSet di posizionamento PALETTA
     //-------------------------------------------------------
     offsX_nod = 0;
     offsY_nod = Magaz[i][1];  // inizializzazione con offset manuale
     if(carico_in_punta==1 && lung_cest<=LUNG_AREA_CARICO) offsY_nod = offsY_nod + ((LUNG_AGV/2) - MezzoLatoY);
     else{
        offsY_nod = offsY_nod + (((LUNG_AGV/2) - MezzoLatoY) + (lung_cest-LUNG_AREA_CARICO));
     }
     //-------------------------------------------------------
     // Disegno di tutte le postazioni
     //-------------------------------------------------------
     for(j=1; j<=posti; j++){
        //-------------------------------------------------------
        // MAGAZZINI MULTI NODO
        // Verifico se è occupato anche solo un nodo da un TGV
        // per visualizzazione trasparente solo con TGV sotto
        //-------------------------------------------------------
        if(tgv_su_fila==false){
           for(z=1; z<=posti; z++){
              if(M.m[i]->Pos[z].NodoIng<1                   ) continue;
              if(M.m[i]->Pos[z].NodoIng>TOTPUNTI            ) continue;
              if(N.nodo_busy[M.m[i]->Pos[z].NodoIng]<1      ) continue;
              if(N.nodo_busy[M.m[i]->Pos[z].NodoIng]>MAXAGV ) continue;
              tgv_su_fila=true;
              break;
           }
        }
        if(tgv_su_fila==false){
           for(z=1; z<=posti; z++){
              if(M.m[i]->Pos[z].NodoOut<1                   ) continue;
              if(M.m[i]->Pos[z].NodoOut>TOTPUNTI            ) continue;
              if(N.nodo_busy[M.m[i]->Pos[z].NodoOut]<1      ) continue;
              if(N.nodo_busy[M.m[i]->Pos[z].NodoOut]>MAXAGV ) continue;
              tgv_su_fila=true;
              break;
           }
        }
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsX = offsX_nod;
        offsY = offsY_nod + ((j-1) * (lung_cest+dist_tra_palette)) + offset_da_nodo;
        //--------------------------------------
        // Recupero eventuale nodo posizione
        //--------------------------------------
        nodo_p = M.m[i]->Pos[j].NodoIng;
        if(nodo_p>0 && nodo_p<=TOTPUNTI){
           xcen  = ((Punti[nodo_p][0]-offset[1][0]));            // pixel x del nodo occupato
           ycen  = ((Punti[nodo_p][1]-offset[1][1]));            // pixel y del nodo occupato
           offsY = offsY_nod;
        }
        //--------------------------------------
        // Colori di default
        //--------------------------------------
        ColBA = clSFONDO;
        ColBB = clSFONDO;
        ColP  = clSFONDO;
        //--------------------------------------
        // Aggiornamento presenza in magazzino
        //--------------------------------------
        PresA = M.m[i]->Pos[j].Presenza[1];      // Presenza cestone ALTO
        PresB = M.m[i]->Pos[j].Presenza[0];      // Presenza cestone BASSO
      //IDA   = M.m[i]->Pos[j].Box[1].ID;        // IDBox del cestone ALTO
      //IDB   = M.m[i]->Pos[j].Box[0].ID;        // IDBox del cestone BASSO
        //--------------------------------------
        // Visualizzazione tanti riquadri
        // quante le tot posizini in presa
        //--------------------------------------
        for(z=0; z<Magaz[i][2]; z++){
           //-------------------------------------------------------
           // Calcolo coordinate grafiche del centro paletta
           //-------------------------------------------------------
           offsX = (((larg_cest * (Magaz[i][2]-z))/2) - MezzoLatoX) - (z*(MezzoLatoX));
           //-------------------------------------------------------
           // ipotenusa del triangolo rettangolo i cui lati sono
           // gli offset del centro paletta dal centro navetta
           //-------------------------------------------------------
           radB  = (long double)(atanl( offsX / offsY ));
           SinCos(radB, sinB, cosB);
           hip   = offsY / cosB;
           //-------------------------------------------------------
           // Calcolo nell'angolo di rotazione del centro paletta
           // come sommatoria dei gradi del TGV + l'angolo di
           // rotazione dal centro TGV al centro paletta
           //-------------------------------------------------------
           radC = radA + radB;
           SinCos(radC, sinC, cosC);
           //-------------------------------------------------------
           // Offset in X,Y del centro PALLET rispetto al centro TGV
           //-------------------------------------------------------
           ComponenteYOffset = hip * cosC;
           ComponenteXOffset = hip * sinC;

           xpal = xcen-(int)(ComponenteXOffset);
           ypal = ycen-(int)(ComponenteYOffset);

           //-------------------------------------------------------
           // Recupero coordinate vertici box per visualizzazione
           //-------------------------------------------------------
           radB = (long double)(atanl( MezzoLatoY / MezzoLatoX ));

           //-------------------------------------------------------
           // Per una corretta visualizzazione in modalità BOBINE
           // il quadrato su cui viene costruita, non può essere
           // inclinato come la navetta ma deve essere sempre
           // ortogonale allo schermo per evitare deformazioni
           //-------------------------------------------------------
           switch(tipo_carico){
              case  0: radB1 = radA + radB;       // rettangolo
                       radB2 = radA - radB;
                       break;
              case  1: radB1 =    0 + radB;       // ellisse
                       radB2 =    0 - radB;
                       break;
              default: radB1 = radA + radB;       // rettangolo
                       radB2 = radA - radB;
                       break;
           }

           SinCos(radB1, sinB1, cosB1);
           SinCos(radB2, sinB2, cosB2);

           MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

           ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
           ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
           ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
           ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

           xinc1 = (int) (xpal-(int)(ComponenteX1));
           xinc2 = (int) (xpal+(int)(ComponenteX2));
           xinc3 = (int) (xpal+(int)(ComponenteX1));
           xinc4 = (int) (xpal-(int)(ComponenteX2));

           yinc1 = (int) (ypal+(int)(ComponenteY1));
           yinc2 = (int) (ypal-(int)(ComponenteY2));
           yinc3 = (int) (ypal-(int)(ComponenteY1));
           yinc4 = (int) (ypal+(int)(ComponenteY2));

           //-----------------------------------------------
           // Aggiustamenti dovuti ad arrotondamenti
           //-----------------------------------------------
           if(grd==90 || grd==270){
              offs1 = abs(xinc4-xinc3);
              if(offs1<3) xinc4=xinc3;
              offs1 = abs(xinc2-xinc1);
              if(offs1<3) xinc2=xinc1;
              offs1 = abs(yinc4-yinc1);
              if(offs1<3) yinc4=yinc1;
              offs1 = abs(yinc3-yinc2);
              if(offs1<3) yinc3=yinc2;
           }
           if(grd==0 || grd==180){
              offs1 = abs(xinc4-xinc1);
              if(offs1<3) xinc4=xinc1;
              offs1 = abs(xinc3-xinc2);
              if(offs1<3) xinc3=xinc2;
              offs1 = abs(yinc2-yinc1);
              if(offs1<3) yinc2=yinc1;
              offs1 = abs(yinc4-yinc3);
              if(offs1<3) yinc4=yinc3;
           }

           //-----------------------------------------------
           // Disegno poligono postazione singola
           //-----------------------------------------------
           points[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
           points[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
           points[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
           points[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);

           pointA[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
           pointA[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
           pointA[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);

           pointB[0] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
           pointB[1] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);
           pointB[2] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);

           //-----------------------------------------------
           // Colore Interno
           //-----------------------------------------------
           GrpDestA  = NN;
           GrpDestB  = NN;
           TipoPresA = LIBERA;
           TipoPresB = LIBERA;
           if(PresA==true){TipoPresA = M.m[i]->Pos[j].Box[1].StatoLav; GrpDestA=M.m[i]->Pos[j].Box[1].GruppoDest;}
           if(PresB==true){TipoPresB = M.m[i]->Pos[j].Box[0].StatoLav; GrpDestB=M.m[i]->Pos[j].Box[0].GruppoDest;}
           //-----------------------------------------------
           // Verifico se in ogni posizione di presa ci
           // sono 1 o più pallet
           //-----------------------------------------------
           TotPal=TotPalA=TotPalB=0;
           for(k=0; k<PIANIINPRESA; k++){
              if(M.m[i]->Pos[j].Box[1].Presa[z][k]==true) TotPalA++;
              if(M.m[i]->Pos[j].Box[0].Presa[z][k]==true) TotPalB++;
              //----------------------------------------------------
              // ### A.L. 13/10/2017 ###
              // Se presente 3° livello visualizza il "2A" / "2B"
              // solo se non possibili prese a 2 piani!
              //----------------------------------------------------
              if( PianiInPresa<2 ){
                 if(M.m[i]->Pos[j].Box[2].Presa[z][k]==true) TotPalA++;
              }
           }

           //-----------------------------------------------
           // Scelta colori contorno
           //-----------------------------------------------
           ColP = clLINEE;
           //-----------------------------------------------
           // Recupero il colore dalla Gruppo
           //-----------------------------------------------
           if(gruppo!=NN && P.GruppiBaie[gruppo].Col!=0) ColP=P.GruppiBaie[gruppo].Col;
           //-----------------------------------------------
           // SOLO EMERGENZA
           //-----------------------------------------------
           if(M.m[i]->PerEmergenza==1) ColP=clRed;
           //-----------------------------------------------
           // Colori contorno
           // TEMPORANEO = yellow
           // ESCLUSIONE = fuchsia
           //-----------------------------------------------
           if(tipo==MG_TEMPORANEO    ) ColP=clYellow;
           if(tipo==MG_ESCLUSO       ) ColP=clFuchsia;
           //-----------------------------------------------
           // Recupero colori interni
           //-----------------------------------------------
           if(PresA || PresB){
              DisegnaBox(TipoPresA, TipoPresB, &ColBA, &ColBB);
              //-----------------------------------------------
              // PALLET IN SPOSTAMENTO
              //-----------------------------------------------
              if(!Lampeggio && M.m[i]->Gruppo!=NN && (GrpDestA!=NN || GrpDestB!=NN)){
                 if(GrpDestA!=NN && M.m[i]->Gruppo!=GrpDestA) ColBA=clSFONDO;
                 if(GrpDestB!=NN && M.m[i]->Gruppo!=GrpDestB) ColBB=clSFONDO;
              }
           }
           //-----------------------------------------------
           // Rettangolo nero per TGV scarico
           //-----------------------------------------------
           if(!PresA && !PresB){
              Sfondo->Canvas->Pen->Color   = ColP;          // sfondo NERO per il Pallet
              Sfondo->Canvas->Brush->Color = clSFONDO;
              //-----------------------------------------------
              // visualizzazione trasparente FISSA, questo
              // permette di vedere il TGV che si muove sotto
              //-----------------------------------------------
              Sfondo->Canvas->Brush->Style = bsSolid;
              if(VisualizzaZoneImpianto==true || tgv_su_fila==true) Sfondo->Canvas->Brush->Style = bsClear;
              //-----------------------------------------------
              // Visualizzazione in base al tipo di carico
              //-----------------------------------------------
              switch(tipo_carico){
                 case  0: Sfondo->Canvas->Polygon(points, 3);
                          break;
                 case  1: Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                          break;
                 default: Sfondo->Canvas->Polygon(points, 3);
                          break;
              }
           }
           //-----------------------------------------------
           // Triangolo colorato per ogni posizione
           //------------------------------------------
           else{
              //--------------------------------------------
              // GESTIONE DOPPIA FORCA
              // Visualizzazione presenza pallet su doppia
              // forca
              //--------------------------------------------
              if(GestioneDoppiaForcaTGV!=0 || abilita_piani!=0){
                 //------------------------------------
                 // Visualizzazione in base al tipo
                 // di carico
                 //------------------------------------
                 switch(tipo_carico){
                    // RETTANGOLO (pallet)
                    case  0: // POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                             if(TotPalB!=0) Sfondo->Canvas->Brush->Color = ColBB;
                             else           Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(tgv_su_fila==true     ) Sfondo->Canvas->Brush->Style = bsDiagCross;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone BASSA
                             Sfondo->Canvas->Polygon(pointB, 2);
                             // POSIZIONE ALTA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX ALTO
                             if(TotPalA!=0) Sfondo->Canvas->Brush->Color = ColBA;
                             else           Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(tgv_su_fila==true     ) Sfondo->Canvas->Brush->Style = bsDiagCross;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone ALTA
                             Sfondo->Canvas->Polygon(pointA, 2);
                             //------------------------------------
                             // Croce nera per indica più di 2
                             // pallet sovrapposti
                             //------------------------------------
                             if(TotPalB>1 && TotPalA>1){
                                Sfondo->Canvas->Pen->Color = ColP;
                                Sfondo->Canvas->MoveTo(xinc2/Rap*Rid, yinc2/Rap*Rid);
                                Sfondo->Canvas->LineTo(xinc4/Rap*Rid, yinc4/Rap*Rid);
                              //Sfondo->Canvas->MoveTo(xinc1/Rap*Rid, yinc1/Rap*Rid);  // aggiungo solo la riga mancante per completare la croce
                              //Sfondo->Canvas->LineTo(xinc3/Rap*Rid, yinc3/Rap*Rid);
                             }
                             break;
                    // ELLISSE (bobina)
                    case  1: Sfondo->Canvas->Brush->Color = clSFONDO;
                             if(TotPalB!=0){Sfondo->Canvas->Pen->Color = ColP;     Sfondo->Canvas->Brush->Color = ColBB;}  // FORCA "B"
                             else          {Sfondo->Canvas->Pen->Color = clSFONDO; Sfondo->Canvas->Brush->Color = ColBA;}  // FORCA "A"
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             // CERCHIO ESTERNO
                             Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             //------------------------------------------------------
                             // Visualizzazione cerchi concentrici per quante
                             // presenze sono a bordo
                             //------------------------------------------------------
                             // FORCA "B"
                             TotPal      = TotPalB+TotPalA;
                             MezzoLatoX1 = MezzoLatoX;
                             MezzoLatoY1 = MezzoLatoY;
                             diff        = MezzoLatoX1/TotPal;
                             for(k=1; k<TotPal; k++){        // parto da "1" perchè il cerchio esterno è già fatto
                                MezzoLatoX1 = MezzoLatoX1-diff;
                                MezzoLatoY1 = MezzoLatoY1-diff;
                                MezzaDiagonale1 = (long double)(sqrt((MezzoLatoX1*MezzoLatoX1)+(MezzoLatoY1*MezzoLatoY1)));

                                ComponenteX1 = (long double) ( MezzaDiagonale1*cosB1 );
                                ComponenteX2 = (long double) ( MezzaDiagonale1*cosB2 );
                                ComponenteY1 = (long double) ( MezzaDiagonale1*sinB1 );
                                ComponenteY2 = (long double) ( MezzaDiagonale1*sinB2 );

                                xinc1 = (int) (xpal-(int)(ComponenteX1));
                                xinc2 = (int) (xpal+(int)(ComponenteX2));
                                xinc3 = (int) (xpal+(int)(ComponenteX1));
                                xinc4 = (int) (xpal-(int)(ComponenteX2));

                                yinc1 = (int) (ypal+(int)(ComponenteY1));
                                yinc2 = (int) (ypal-(int)(ComponenteY2));
                                yinc3 = (int) (ypal-(int)(ComponenteY1));
                                yinc4 = (int) (ypal+(int)(ComponenteY2));
                                //-----------------------------------------------
                                // Aggiustamenti dovuti ad arrotondamenti
                                //-----------------------------------------------
                                if(grd==90 || grd==270){
                                   offs1 = abs(xinc4-xinc3);
                                   if(offs1<3) xinc4=xinc3;
                                   offs1 = abs(xinc2-xinc1);
                                   if(offs1<3) xinc2=xinc1;
                                   offs1 = abs(yinc4-yinc1);
                                   if(offs1<3) yinc4=yinc1;
                                   offs1 = abs(yinc3-yinc2);
                                   if(offs1<3) yinc3=yinc2;
                                }
                                if(grd==0 || grd==180){
                                   offs1 = abs(xinc4-xinc1);
                                   if(offs1<3) xinc4=xinc1;
                                   offs1 = abs(xinc3-xinc2);
                                   if(offs1<3) xinc3=xinc2;
                                   offs1 = abs(yinc2-yinc1);
                                   if(offs1<3) yinc2=yinc1;
                                   offs1 = abs(yinc4-yinc3);
                                   if(offs1<3) yinc4=yinc3;
                                }

                                // --- disegno riquadro pallet con Forca Singola
                                points[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
                                points[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
                                points[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
                                points[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);

                                Sfondo->Canvas->Pen->Color   = clSFONDO;
                                if(k<=TotPalB) Sfondo->Canvas->Brush->Color = ColBB; // FORCA "B"
                                else           Sfondo->Canvas->Brush->Color = ColBA; // FORCA "A"
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                // CERCHIO INTERNO
                                Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             }
                             break;
                    // RETTANGOLO (pallet)
                    default: // POSIZIONE BASSA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                             if(TotPalB!=0) Sfondo->Canvas->Brush->Color = ColBB;
                             else           Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(tgv_su_fila==true     ) Sfondo->Canvas->Brush->Style = bsDiagCross;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone BASSA
                             Sfondo->Canvas->Polygon(pointB, 2);
                             // POSIZIONE ALTA
                             Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX ALTO
                             if(TotPalA!=0) Sfondo->Canvas->Brush->Color = ColBA;
                             else           Sfondo->Canvas->Brush->Color = clSFONDO;
                             Sfondo->Canvas->Brush->Style = bsSolid;
                             if(tgv_su_fila==true     ) Sfondo->Canvas->Brush->Style = bsDiagCross;
                             if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                             // triangolo posizone ALTA
                             Sfondo->Canvas->Polygon(pointA, 2);
                             //------------------------------------
                             // Croce nera per indica più di 2
                             // pallet sovrapposti
                             //------------------------------------
                             if(TotPalB>1 && TotPalA>1){
                                Sfondo->Canvas->Pen->Color = ColP;
                                Sfondo->Canvas->MoveTo(xinc2/Rap*Rid, yinc2/Rap*Rid);
                                Sfondo->Canvas->LineTo(xinc4/Rap*Rid, yinc4/Rap*Rid);
                              //Sfondo->Canvas->MoveTo(xinc1/Rap*Rid, yinc1/Rap*Rid);  // aggiungo la riga mancante per completare la croce
                              //Sfondo->Canvas->LineTo(xinc3/Rap*Rid, yinc3/Rap*Rid);
                             }
                             break;
                 }
                 //------------------------------------
                 // VISUALIZZAZIONE SU PALLET A MAG.
                 // Stampa stringa su pallet
                 //------------------------------------
                 if( Rid>=RID_NUMERI && tipo_carico!=1 && (PresB==true || PresA==true) && (TotPalB<2 || TotPalA<2)){
                    if(TipoPresB!=PRES_VUOTO && TotPalB!=0){
                       if(TotPalB==1){
                          if(M.m[i]->Pos[j].Box[0].Basso==false) strcpy(idbox, "A");
                          else strcpy(idbox, "B");
                       }
                       if(TotPalB==2){
                          if(M.m[i]->Pos[j].Box[0].Basso==false) strcpy(idbox, "2A");
                          else strcpy(idbox, "2B");
                       }
                       if(sviluppo==0) StampaStringa(Sfondo, ((xinc4+11)/Rap*Rid), ((yinc4+30)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==1) StampaStringa(Sfondo, ((xinc4-39)/Rap*Rid), ((yinc4-40)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==2) StampaStringa(Sfondo, ((xinc4+15)/Rap*Rid), ((yinc4-40)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==3) StampaStringa(Sfondo, ((xinc4-41)/Rap*Rid), ((yinc4+15)/Rap*Rid), idbox, clBlack);
                    }
                    if(TipoPresA!=PRES_VUOTO && TotPalA!=0){
                       if(TotPalA==1){
                          if(M.m[i]->Pos[j].Box[1].Basso==false) strcpy(idbox, "A");
                          else strcpy(idbox, "B");
                       }
                       if(TotPalA==2){
                          if(M.m[i]->Pos[j].Box[1].Basso==false) strcpy(idbox, "2A");
                          else strcpy(idbox, "2B");
                       }
                       if(sviluppo==0) StampaStringa(Sfondo, ((xinc2-39)/Rap*Rid), ((yinc2-40)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==1) StampaStringa(Sfondo, ((xinc2+11)/Rap*Rid), ((yinc2+30)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==2) StampaStringa(Sfondo, ((xinc2-41)/Rap*Rid), ((yinc2+15)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==3) StampaStringa(Sfondo, ((xinc2+15)/Rap*Rid), ((yinc2-40)/Rap*Rid), idbox, clBlack);
                    }
                 }
              }
              //--------------------------------------------
              // GESTIONE FORCA SINGOLA
              // Visualizzazione presenza pallet singola
              //--------------------------------------------
              else{
                 // POSIZIONE BASSA
                 Sfondo->Canvas->Pen->Color   = ColP;          // aspetto reale del BOX BASSO
                 Sfondo->Canvas->Brush->Color = ColBB;
                 if(TotPalB!=0) Sfondo->Canvas->Brush->Color = ColBB;
                 else           Sfondo->Canvas->Brush->Color = clSFONDO;
                 Sfondo->Canvas->Brush->Style = bsSolid;
                 if(tgv_su_fila==true     ) Sfondo->Canvas->Brush->Style = bsDiagCross;
                 if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                 //------------------------------------
                 // Visualizzazione in base al tipo
                 // di carico
                 //------------------------------------
                 switch(tipo_carico){
                    case  0: // RETTANGOLO
                             Sfondo->Canvas->Polygon(points, 3);
                             //------------------------------------
                             // Croce nera per indica più di 2
                             // pallet sovrapposti
                             //------------------------------------
                             if(TotPalB>1){
                                Sfondo->Canvas->Pen->Color = ColP;
                                Sfondo->Canvas->MoveTo(xinc1/Rap*Rid, yinc1/Rap*Rid);
                                Sfondo->Canvas->LineTo(xinc3/Rap*Rid, yinc3/Rap*Rid);
                                if(TotPalB>2){
                                   Sfondo->Canvas->MoveTo(xinc2/Rap*Rid, yinc2/Rap*Rid);
                                   Sfondo->Canvas->LineTo(xinc4/Rap*Rid, yinc4/Rap*Rid);
                                }
                             }
                             break;
                    case  1: // CERCHIO ESTERNO
                             Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             //------------------------------------------------------
                             // Visualizzazione cerchi concentrici per quante
                             // presenze sono a bordo
                             //------------------------------------------------------
                             // FORCA "B"
                             TotPal      = TotPalB+TotPalA;
                             MezzoLatoX1 = MezzoLatoX;
                             MezzoLatoY1 = MezzoLatoY;
                             diff        = MezzoLatoX1/TotPal;
                             for(k=1; k<TotPal; k++){        // parto da "1" perchè il cerchio esterno è già fatto
                                MezzoLatoX1 = MezzoLatoX1-diff;
                                MezzoLatoY1 = MezzoLatoY1-diff;
                                MezzaDiagonale1 = (long double)(sqrt((MezzoLatoX1*MezzoLatoX1)+(MezzoLatoY1*MezzoLatoY1)));

                                ComponenteX1 = (long double) ( MezzaDiagonale1*cosB1 );
                                ComponenteX2 = (long double) ( MezzaDiagonale1*cosB2 );
                                ComponenteY1 = (long double) ( MezzaDiagonale1*sinB1 );
                                ComponenteY2 = (long double) ( MezzaDiagonale1*sinB2 );

                                xinc1 = (int) (xpal-(int)(ComponenteX1));
                                xinc2 = (int) (xpal+(int)(ComponenteX2));
                                xinc3 = (int) (xpal+(int)(ComponenteX1));
                                xinc4 = (int) (xpal-(int)(ComponenteX2));

                                yinc1 = (int) (ypal+(int)(ComponenteY1));
                                yinc2 = (int) (ypal-(int)(ComponenteY2));
                                yinc3 = (int) (ypal-(int)(ComponenteY1));
                                yinc4 = (int) (ypal+(int)(ComponenteY2));
                                //-----------------------------------------------
                                // Aggiustamenti dovuti ad arrotondamenti
                                //-----------------------------------------------
                                if(grd==90 || grd==270){
                                   offs1 = abs(xinc4-xinc3);
                                   if(offs1<3) xinc4=xinc3;
                                   offs1 = abs(xinc2-xinc1);
                                   if(offs1<3) xinc2=xinc1;
                                   offs1 = abs(yinc4-yinc1);
                                   if(offs1<3) yinc4=yinc1;
                                   offs1 = abs(yinc3-yinc2);
                                   if(offs1<3) yinc3=yinc2;
                                }
                                if(grd==0 || grd==180){
                                   offs1 = abs(xinc4-xinc1);
                                   if(offs1<3) xinc4=xinc1;
                                   offs1 = abs(xinc3-xinc2);
                                   if(offs1<3) xinc3=xinc2;
                                   offs1 = abs(yinc2-yinc1);
                                   if(offs1<3) yinc2=yinc1;
                                   offs1 = abs(yinc4-yinc3);
                                   if(offs1<3) yinc4=yinc3;
                                }

                                // --- disegno riquadro pallet con Forca Singola
                                points[0] = Point(xinc1/Rap*Rid, yinc1/Rap*Rid);
                                points[1] = Point(xinc2/Rap*Rid, yinc2/Rap*Rid);
                                points[2] = Point(xinc3/Rap*Rid, yinc3/Rap*Rid);
                                points[3] = Point(xinc4/Rap*Rid, yinc4/Rap*Rid);

                                Sfondo->Canvas->Pen->Color   = clSFONDO;
                                Sfondo->Canvas->Brush->Color = ColBB; // FORCA "B"
                                Sfondo->Canvas->Brush->Style = bsSolid;
                                if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                                // CERCHIO INTERNO
                                Sfondo->Canvas->Ellipse(points[0].x, points[0].y, points[2].x, points[2].y);
                             }
                             break;
                    default: Sfondo->Canvas->Polygon(points, 3);
                             //------------------------------------
                             // Croce nera per indica più di 2
                             // pallet sovrapposti
                             //------------------------------------
                             if(TotPalB>1){
                                Sfondo->Canvas->Pen->Color = ColP;
                                Sfondo->Canvas->MoveTo(xinc1/Rap*Rid, yinc1/Rap*Rid);
                                Sfondo->Canvas->LineTo(xinc3/Rap*Rid, yinc3/Rap*Rid);
                                if(TotPalB>2){
                                   Sfondo->Canvas->MoveTo(xinc2/Rap*Rid, yinc2/Rap*Rid);
                                   Sfondo->Canvas->LineTo(xinc4/Rap*Rid, yinc4/Rap*Rid);
                                }
                             }
                             break;
                 }

                 //§§§
                 //----------------------------------------
                 // Andrea 18/05/2017
                 // VISUALIZZAZIONE SU PALLET A MAG.
                 // Stampa stringa su pallet anche singolo
                 //----------------------------------------
                 if( Rid>=RID_NUMERI && tipo_carico!=1 && (PresB==true || PresA==true) && (TotPalB<2 || TotPalA<2)){
                    if(TipoPresB!=PRES_VUOTO && TotPalB!=0){
                       if(TotPalB==1){
                          if(M.m[i]->Pos[j].Box[0].Basso==false) strcpy(idbox, "A");
                          else strcpy(idbox, "B");
                       }
                       if(TotPalB==2){
                          if(M.m[i]->Pos[j].Box[0].Basso==false) strcpy(idbox, "2A");
                          else strcpy(idbox, "2B");
                       }
                       if(sviluppo==0) StampaStringa(Sfondo, ((xinc4+11)/Rap*Rid), ((yinc4+30)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==1) StampaStringa(Sfondo, ((xinc4-39)/Rap*Rid), ((yinc4-40)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==2) StampaStringa(Sfondo, ((xinc4+15)/Rap*Rid), ((yinc4-40)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==3) StampaStringa(Sfondo, ((xinc4-41)/Rap*Rid), ((yinc4+15)/Rap*Rid), idbox, clBlack);
                    }
                    if(TipoPresA!=PRES_VUOTO && TotPalA!=0){
                       if(TotPalA==1){
                          if(M.m[i]->Pos[j].Box[1].Basso==false) strcpy(idbox, "A");
                          else strcpy(idbox, "B");
                       }
                       if(TotPalA==2){
                          if(M.m[i]->Pos[j].Box[1].Basso==false) strcpy(idbox, "2A");
                          else strcpy(idbox, "2B");
                       }
                       if(sviluppo==0) StampaStringa(Sfondo, ((xinc2-39)/Rap*Rid), ((yinc2-40)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==1) StampaStringa(Sfondo, ((xinc2+11)/Rap*Rid), ((yinc2+30)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==2) StampaStringa(Sfondo, ((xinc2-41)/Rap*Rid), ((yinc2+15)/Rap*Rid), idbox, clBlack);
                       if(sviluppo==3) StampaStringa(Sfondo, ((xinc2+15)/Rap*Rid), ((yinc2-40)/Rap*Rid), idbox, clBlack);
                    }
                 }
                 //§§§

              }
           }
        }
     }
     //------------------------------------------
     // Visualizzazione del numero del magazzino
     //------------------------------------------
     if( Rid>=RID_NUMERI ){
        //-------------------------------------------------------
        // Offset numeri in base al quadrante di visualizzazione
        //-------------------------------------------------------
        offnum = 20;
        if(grd>130 && grd<200) offnum = 80;
        if(grd>200 && grd<300) offnum = 60;
        if(grd>300 && grd<330) offnum = 40;
        //-------------------------------------------------------
        // OffSet di posizionamento PALETTA
        //-------------------------------------------------------
        offsY = offsY_nod;
        //-------------------------------------------------------
        // Incremento offset posizione TGV da nodo
        //-------------------------------------------------------
        offsY = offsY + ((posti-1) * (lung_cest+dist_tra_palette)) + offset_da_nodo;
        //-------------------------------------------------------
        // MAGAZZINI MULTI NODO
        // Il riferimento è sempre il nodo della posizione
        //-------------------------------------------------------
        nodo_p = M.m[i]->Pos[posti].NodoIng;
        if(nodo_p>0 && nodo_p<=TOTPUNTI){
           xcen  = ((Punti[nodo_p][0]-offset[1][0]));            // pixel x del nodo occupato
           ycen  = ((Punti[nodo_p][1]-offset[1][1]));            // pixel y del nodo occupato
           offsY = offsY_nod;
        }
        //-------------------------------------------------------
        // Determinato il centro dell'ultima paletta devo sommare
        // altro mezzo box per uscire dall'ingombro con la
        // scrittura del numero
        //-------------------------------------------------------
        offsY = offsY + MezzoLatoY + offnum;         // il numero deve essere sempre fuori dalla palette
        //-------------------------------------------------------
        // Calcolo coordinate grafiche del centro paletta
        //-------------------------------------------------------
        offsX = 0;   // rif. centro nodo
        //-------------------------------------------------------
        // ipotenusa del triangolo rettangolo i cui lati sono
        // gli offset del centro paletta dal centro navetta
        //-------------------------------------------------------
        radB  = (long double)(atanl( offsX / offsY ));
        SinCos(radB, sinB, cosB);
        hip   = offsY / cosB;
        //-------------------------------------------------------
        // Calcolo nell'angolo di rotazione del centro paletta
        // come sommatoria dei gradi del TGV + l'angolo di
        // rotazione dal centro TGV al centro paletta
        //-------------------------------------------------------
        radC = radA + radB;
        SinCos(radC, sinC, cosC);
        //-------------------------------------------------------
        // Offset in X,Y del centro PALLET rispetto al centro TGV
        //-------------------------------------------------------
        ComponenteYOffset = hip * cosC;
        ComponenteXOffset = hip * sinC;
        xnum = xcen-(int)(ComponenteXOffset);
        ynum = ycen-(int)(ComponenteYOffset);
        StampNum(FormLayout->Sfondo, ((xnum/Rap)*Rid), ((ynum/Rap)*Rid), num, clFuchsia, TRUE, Punti[nodo][3]);
        //------------------------------------------
        // Scrittura SIGLA MAGAZZINI
        //------------------------------------------
        strcpy(sigla, "");
        clSigla = clWhite;
        if(M.m[i]->Assegnazione!=ASS_DINAMICA && M.m[i]->Assegnazione!=PRES_VUOTO){
           switch(M.m[i]->Assegnazione){
              case  PRES_LAV1:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD1);
                                else                           strcpy(sigla, ASS_LAV1);
                                clSigla = (TColor)(COLOR_PRES_STAT1);
                                break;
              case  PRES_LAV2:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD2);
                                else                           strcpy(sigla, ASS_LAV2);
                                clSigla = (TColor)(COLOR_PRES_STAT2);
                                break;
              case  PRES_LAV3:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD3);
                                else                           strcpy(sigla, ASS_LAV3);
                                clSigla = (TColor)(COLOR_PRES_STAT3);
                                break;
              case  PRES_LAV4:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD4);
                                else                           strcpy(sigla, ASS_LAV4);
                                clSigla = (TColor)(COLOR_PRES_STAT4);
                                break;
              case  PRES_LAV5:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD5);
                                else                           strcpy(sigla, ASS_LAV5);
                                clSigla = (TColor)(COLOR_PRES_STAT5);
                                break;
              case  PRES_LAV6:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD6);
                                else                           strcpy(sigla, ASS_LAV6);
                                clSigla = (TColor)(COLOR_PRES_STAT6);
                                break;
              case  PRES_LAV7:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD7);
                                else                           strcpy(sigla, ASS_LAV7);
                                clSigla = (TColor)(COLOR_PRES_STAT7);
                                break;
              case  PRES_LAV8:  if(AbbinaCodiceStatoBox==true) sprintf(sigla, "%02d", ASS_COD8);
                                else                           strcpy(sigla, ASS_LAV8);
                                clSigla = (TColor)(COLOR_PRES_STAT8);
                                break;
                      default:  strcpy(sigla, "##");
                                break;
           }
           Sfondo->Canvas->Font->Color  = clSigla;
           Sfondo->Canvas->Brush->Color = clSFONDO;
           Sfondo->Canvas->Brush->Style = bsSolid;
           //---------------------------------------------
           // Scrittura sigla su sfondo grafico
           //---------------------------------------------
           if(xs!=0 || ys!=0) Sfondo->Canvas->TextOut(xs, ys, sigla);
        }
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       linea_punti()
// ----------------------------------------------
// Questa funzione verifica se Source e Dest sono occupati dallo stesso
// AGV, allora disegna la linea del colore AGV
//
void TFormLayout::linea_punti( int source, int dest, TColor col )
/*************************************************/
{
  int x1, x2, y1, y2;

  //-------------------------------------------------------
  // NODI DELL'IMPIANTO
  //-------------------------------------------------------
  if(source<1       ) return;
  if(source>TOTPUNTI) return;
  if(dest<1         ) return;
  if(dest>TOTPUNTI  ) return;

  //-------------------------------------------------------
  // 30/04/97 Disegna con l'offset relativo alla pagina
  //-------------------------------------------------------
  x1 = ((Punti[source][0]-offset[ Punti[ source ][4]][0])/Rap)*Rid ;
  y1 = ((Punti[source][1]-offset[ Punti[ source ][4]][1])/Rap)*Rid ;
  x2 = ((Punti[dest][0]  -offset[ Punti[  dest  ][4]][0])/Rap)*Rid ;
  y2 = ((Punti[dest][1]  -offset[ Punti[  dest  ][4]][1])/Rap)*Rid ;

  Sfondo->Canvas->Pen->Color = col;
  Sfondo->Canvas->MoveTo( x1, y1 );
  Sfondo->Canvas->LineTo( x2, y2 );
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//                CurvaGenerica()
// ----------------------------------------------
void  TFormLayout::CurvaGenerica( int X1, int Y1, int X2, int Y2, int tipo, int raggio, TColor col, int Riempimento )
/*************************************************/
{
//char buffer[200];
  int x1, y1, x2, y2;
  int xP1, yP1, xP2, yP2, xP3, yP3, xP4, yP4, xP5, yP5;

  long double distX, distY;
  long double dist, h;
  long double tn1, rd1, rd2, sn2, cos2;
  long double ComponenteX, ComponenteY;


  long double X_cen, Y_cen;    // Coordinate del centro della circonferenza
  long double X_P0, Y_P0;      // Coordinate della perpendicolare sul centro corda
  long double raggioDouble;    // raggio con precisione maggiore


  //-------------------------------------------------------
  // Coordinate degli estremi dell curva
  //-------------------------------------------------------
  x1 = X1;
  y1 = Y1;
  x2 = X2;
  y2 = Y2;

  //----------------------------------------------
  // Disegno l'arco di circonferenza
  //----------------------------------------------
  //-------------------------------------------------------
  // CALCOLO CORDA
  // Se i nodi si trovano in retta la distanza viene
  // calcolata con il teorema di pitagora.
  //-------------------------------------------------------
  distX=abs(x1-x2);
  distY=abs(y1-y2);
  dist = (long double)(sqrt_algo((distX*distX)+(distY*distY)));
  if( dist==0 ) return;

  //---------------------------------------------
  // Calcolo dell'angolo ALPHA
  // racchiuso tra la corda che unisce i 2 punti
  // e le ascisse
  //---------------------------------------------
  if(x1!=x2 && y1!=y2 ){
     tn1  = (long double)(distY)/(long double)(distX);
     rd1  = (long double)(atanl(tn1));
  }
  if( y1==y2 )  rd1  = 0;
  if( x1==x2 )  rd1  = (long double)(M_PI/2);

  //---------------------------------------------
  // Calcolo automatico del RAGGIO se NECESSARIO
  // Dando come VINCOLO che l'arco in almeno
  // uno dei 2 punti sia tangente alle
  // ASCISSE o alle ORDINATE
  //---------------------------------------------
  raggioDouble = (long double) (raggio);

  if( raggioDouble==0 ){
     rd2= (M_PI/2) - rd1;
     sn2  = (long double)(sinl(rd2));
     if(sn2!=0) raggioDouble = (double)((dist/2)/sn2);
     else raggioDouble = dist;
  }
  // Il raggio ipotenusa del triangolo deve essere > cateto
  if( raggioDouble<= dist/2 ) raggioDouble = dist/2 + 1;

  //--------------------------------------------
  // Calcolo di rd2 vedi documento 19/02/2003
  // 180° = PI   =  3.14159265358979
  //  90  = PI/2 =  1.57079632679489
  //--------------------------------------------
  rd2 =  (long double) ((long double)(M_PI/2)-rd1); //90- rd1;

  h = (double)(sqrt_algo((raggioDouble*raggioDouble)-(dist*dist/4)));

  sn2  = (long double)(sinl(rd2));
  cos2 = (long double)(cosl(rd2));

  ComponenteX = (long double) (h*cos2);
  ComponenteY = (long double) (h*sn2);
  //--------------------------------------------
  // Calcolo delle coordinate del centro curva
  // noto il raggio e un punto
  //--------------------------------------------
  xP3 = x1;
  yP3 = y1;
  xP4 = x2;
  yP4 = y2;
  X_P0 = (long double) (x1-((x1-x2)/2));
  Y_P0 = (long double) (y1-((y1-y2)/2));
  //--------------------------------------------
  // Controllo dei quattro quadranti
  //--------------------------------------------
  switch(tipo){
     //------------------------_----------
     // Quadrante 1   0..90°    |
     //-----------------------------------
     case 1:  X_cen=(long double)( X_P0 - ComponenteX);
              Y_cen=(long double)( Y_P0 + ComponenteY);
              if( x1<x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              //--------------------------------------
              // Coordinate di riempimento curva
              //--------------------------------------
              xP5 = X_P0+5;
              yP5 = Y_P0-5;
              break;
     //----------------------------_------
     // Quadrante 2   90°..180°   |
     //-----------------------------------
     case 2:  X_cen=(long double)( X_P0 + ComponenteX);
              Y_cen=(long double)( Y_P0 + ComponenteY);
              if( x1<x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              //--------------------------------------
              // Coordinate di riempimento curva
              //--------------------------------------
              xP5 = X_P0-5;
              yP5 = Y_P0-5;
              break;
     //-----------------------------------
     // Quadrante 3   180°..270°   |_
     //-----------------------------------
     case 3:  X_cen=(long double)( X_P0 + ComponenteX);
              Y_cen=(long double)( Y_P0 - ComponenteY);
              if( x1>x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              //--------------------------------------
              // Coordinate di riempimento curva
              //--------------------------------------
              xP5 = X_P0-5;
              yP5 = Y_P0+5;
              break;
     //-----------------------------------
     // Quadrante 4   270°..0°  _|
     //-----------------------------------
     case 4:  X_cen=(long double)( X_P0 - ComponenteX);
              Y_cen=(long double)( Y_P0 - ComponenteY);
              if( x1>x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              //--------------------------------------
              // Coordinate di riempimento curva
              //--------------------------------------
              xP5 = X_P0+5;
              yP5 = Y_P0+5;
              break;
     default: X_cen=(long double)( X_P0 - ComponenteX);
              Y_cen=(long double)( Y_P0 + ComponenteY);
              break;
  }
  //--------------------------------------------
  // Calcolo delle coordinate degli spigoli del
  // rettangolo della funzione arc() e dei punti
  // della corda: i suoi P1,P2,P3,P4
  //--------------------------------------------
  xP1 = (int) (X_cen + raggioDouble);
  yP1 = (int) (Y_cen + raggioDouble);
  xP2 = (int) (X_cen - raggioDouble);
  yP2 = (int) (Y_cen - raggioDouble);

  //--------------------------------------------
  // Gestione di una curva piena
  //--------------------------------------------
  if(Riempimento){
     Sfondo->Canvas->Pen->Color = clLtGray;
     Sfondo->Canvas->Arc( xP1/Rap*Rid, yP1/Rap*Rid, xP2/Rap*Rid, yP2/Rap*Rid, xP3/Rap*Rid, yP3/Rap*Rid, xP4/Rap*Rid, yP4/Rap*Rid);
     Sfondo->Canvas->MoveTo(xP3/Rap*Rid, yP3/Rap*Rid);
     Sfondo->Canvas->LineTo(xP4/Rap*Rid, yP4/Rap*Rid);
     Sfondo->Canvas->FloodFill(xP5/Rap*Rid, yP5/Rap*Rid, clLtGray, fsBorder);
     Sfondo->Canvas->Pen->Color = col;
     Sfondo->Canvas->MoveTo(xP3/Rap*Rid, yP3/Rap*Rid);
     Sfondo->Canvas->LineTo(xP4/Rap*Rid, yP4/Rap*Rid);
  }
  //--------------------------------------------
  // Contorno curva
  //--------------------------------------------
  Sfondo->Canvas->Pen->Color = col;
  Sfondo->Canvas->Arc( xP1/Rap*Rid, yP1/Rap*Rid, xP2/Rap*Rid, yP2/Rap*Rid, xP3/Rap*Rid, yP3/Rap*Rid, xP4/Rap*Rid, yP4/Rap*Rid);
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//     curva_punti()
// ----------------------------------------------
// Questa funzione verifica se Source e Dest sono occupati dallo stesso
// AGV, allora disegna la linea del colore AGV
//
void TFormLayout::curva_punti( int source, int dest, int tipo, int raggio, TColor col)
/*************************************************/
{
//char buffer[200];
  int x1, y1, x2, y2;
  int xP1, yP1, xP2, yP2, xP3, yP3, xP4, yP4;

  int i;
  long double distX, distY;
  long double dist, h;
  long double tn1, rd1, rd2, sn2, cos2;
  long double ComponenteX, ComponenteY;


  long double X_cen, Y_cen;    // Coordinate del centro della circonferenza
  long double X_P0, Y_P0;      // Coordinate della perpendicolare sul centro corda
  long double raggioDouble;    // raggio con precisione maggiore
  //-------------------------------------------------------
  // 30/04/97 Disegna con l'offset relativo alla pagina
  //-------------------------------------------------------
  x1 = ((Punti[source][0]-offset[1][0]));
  y1 = ((Punti[source][1]-offset[1][1]));
  x2 = ((Punti[ dest ][0]-offset[1][0]));
  y2 = ((Punti[ dest ][1]-offset[1][1]));


  Sfondo->Canvas->Pen->Color = col;

  //----------------------------------------------
  // Disegno l'arco di circonferenza
  //----------------------------------------------
  //----------------------------------
  // CALCOLO CORDA
  // Se i nodi si trovano in retta la
  // distanza viene calcolata con il
  // teorema di pitagora.
  //----------------------------------
  distX=abs(x1-x2);
  distY=abs(y1-y2);
  dist = (long double)(sqrt_algo((distX*distX)+(distY*distY)));

  //---------------------------------------------
  // Calcolo dell'angolo ALPHA
  // racchiuso tra la corda che unisce i 2 punti
  // e le ascisse
  //---------------------------------------------
  if(x1!=x2 && y1!=y2 ){
     tn1  = (long double)(distY)/(long double)(distX);
     rd1  = (long double)(atanl(tn1));
  }
  //---------------------------------------------
  // Se esiste curva con raggio in graphvar.cpp
  // recupero da struttura
  //---------------------------------------------
  if( raggio==0 ){
     //-----------------------------------------------
     // Recupera eventualmente raggio da graphvar.cpp
     //-----------------------------------------------
     for(i=0;;i++){
        if(( source==PercorsoCurva[i][0] && dest  ==PercorsoCurva[i][1]) ||
           ( dest  ==PercorsoCurva[i][0] && source==PercorsoCurva[i][1])){
             raggio = PercorsoCurva[i][3];
             //if( PercorsoCurva[i][4] ==0 ) break;
             break;
        }
        if (raggio) break;
        //-----------------------
        // fine percorso -> esci
        //-----------------------
        if( PercorsoCurva[i][0] ==0 ) break;
        if( PercorsoCurva[i][1] ==0 ) break;
     }
  }
  //---------------------------------------------
  // Calcolo automatico del RAGGIO se NECESSARIO
  // Dando come VINCOLO che l'arco in almeno
  // uno dei 2 punti sia tangente alle
  // ASCISSE o alle ORDINATE
  //---------------------------------------------
  raggioDouble = (long double) (raggio);

  if( raggioDouble==0 ){
     rd2= (M_PI/2) - rd1;
     sn2  = (long double)(sinl(rd2));
     if(sn2!=0) raggioDouble = (double)((dist/2)/sn2);
     else raggioDouble = dist;
  }
  // Il raggio ipotenusa del triangolo deve essere > cateto
  if( raggioDouble<= dist/2 ) raggioDouble = dist/2 + 1;

  //--------------------------------------------
  // Calcolo di rd2 vedi documento 19/02/2003
  // 189° = PI   =  3.14159265358979
  //  90° = PI/2 =  1.57079632679489
  //--------------------------------------------
  //rd2 =  (long double) (1.57079632679489-rd1); //90- rd1;
  rd2 =  (long double) ((M_PI/2)-rd1); //90- rd1;


  h = (double)(sqrt_algo((raggioDouble*raggioDouble)-(dist*dist/4)));

  sn2  = (long double)(sinl(rd2));
  cos2 = (long double)(cosl(rd2));

  ComponenteX = (long double) (h*cos2);
  ComponenteY = (long double) (h*sn2);

  //--------------------------------------------
  // Calcolo delle coordinate del centro curva
  // noto il raggio e un punto
  //--------------------------------------------
  xP3 = x1;
  yP3 = y1;
  xP4 = x2;
  yP4 = y2;
  X_P0 = (long double) (x1-((x1-x2)/2));
  Y_P0 = (long double) (y1-((y1-y2)/2));
  //-----------------------------------
  // Controllo dei quattro quadranti
  //-----------------------------------
  switch(tipo){
     //------------------------_----------
     // Quadrante 1   0..90°    |
     //-----------------------------------
     case 1:  X_cen=(long double)( X_P0 -  ComponenteX);
              Y_cen=(long double)( Y_P0 +  ComponenteY);
              if( x1<x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              break;
     //----------------------------_------
     // Quadrante 2   90°..180°   |
     //-----------------------------------
     case 2:  X_cen=(long double)( X_P0 +  ComponenteX);
              Y_cen=(long double)( Y_P0 +  ComponenteY);
              if( x1<x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              break;
     //-----------------------------------
     // Quadrante 3   180°..270°   |_
     //-----------------------------------
     case 3:  X_cen=(long double)( X_P0 +  ComponenteX);
              Y_cen=(long double)( Y_P0 -  ComponenteY);
              if( x1>x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              break;
     //-----------------------------------
     // Quadrante 4   270°..0°  _|
     //-----------------------------------
     case 4:  X_cen=(long double)( X_P0 -  ComponenteX);
              Y_cen=(long double)( Y_P0 -  ComponenteY);
              if( x1>x2 ){
                 xP3 = x2;
                 yP3 = y2;
                 xP4 = x1;
                 yP4 = y1;
              }
              break;
     default: X_cen=(long double)( X_P0 -  ComponenteX);
              Y_cen=(long double)( Y_P0 +  ComponenteY);
              break;
  }
  //--------------------------------------------
  // Calcolo delle coordinate degli spigoli del
  // rettangolo della funzione arc() e dei punti
  // della corda: i suoi P1,P2,P3,P4
  //--------------------------------------------
  xP1 = (int) (X_cen + raggioDouble);
  yP1 = (int) (Y_cen + raggioDouble);
  xP2 = (int) (X_cen - raggioDouble);
  yP2 = (int) (Y_cen - raggioDouble);

  xP1 = xP1 /Rap*Rid;
  yP1 = yP1 /Rap*Rid;
  xP2 = xP2 /Rap*Rid;
  yP2 = yP2 /Rap*Rid;
  xP3 = xP3 /Rap*Rid;
  yP3 = yP3 /Rap*Rid;
  xP4 = xP4 /Rap*Rid;
  yP4 = yP4 /Rap*Rid;

  Sfondo->Canvas->Arc( xP1, yP1, xP2, yP2, xP3, yP3, xP4, yP4);
}
//---------------------------------------------------------------------------
void TFormLayout::curva_punti_bezier( TPoint *p, short int tot_p, TColor col)
{
  int x,y;
  double t;
  double step;
  double xt,yt;
  double ax,ay;
  double bx,by;
  double cx,cy;
  double dx,dy;
  double fx,fy;

  // CURVE QUADRATICHE
  if(tot_p==2){
     // calcola i coefficienti
     bx = 2 * (p[1].x - p[0].x);
     ax = p[2].x - p[0].x - bx;

     by = 2 * (p[1].y - p[0].y);
     ay = p[2].y - p[0].y - by;


     // cicla tra 0 e 1 con passo step

     t = 0;
     step = 0.01;
     Sfondo->Canvas->Pen->Color = col;
     Sfondo->Canvas->MoveTo(p[0].x, p[0].y);

     while (t < 1) {
        xt = ax * pow(t, 2) + bx * t + p[0].x;
        yt = ay * pow(t, 2) + by * t + p[0].y;

        x = (int) floor(xt + 0.5);
        y = (int) floor(yt + 0.5);

        Sfondo->Canvas->LineTo(x,y);
        t += step;
     }
  }
  // CURVE CUBICHE
  if(tot_p==3){
     // calcola i coefficienti
     cx = 3 * (p[1].x - p[0].x);
     bx = 3 * (p[2].x - p[1].x) - cx;
     ax = p[3].x - p[0].x - cx - bx;

     cy = 3 * (p[1].y - p[0].y);
     by = 3 * (p[2].y - p[1].y) - cy;
     ay = p[3].y - p[0].y - cy - by;


     // cicla tra 0 e 1 con passo step

     t = 0;
     step = 0.01;
     Sfondo->Canvas->Pen->Color = col;
     Sfondo->Canvas->MoveTo(p[0].x, p[0].y);

     while (t < 1) {
        xt = ax * pow(t, 3) + bx * pow(t, 2) + cx * t + p[0].x;
        yt = ay * pow(t, 3) + by * pow(t, 2) + cy * t + p[0].y;

        x = (int) floor(xt + 0.5);
        y = (int) floor(yt + 0.5);

        Sfondo->Canvas->LineTo(x,y);
        t += step;
     }
  }

  return;
}
//---------------------------------------------------------------------------
void TFormLayout::modifica_curva_punti_bezier( TPoint *p, short int tot_p)
{
  int x,y,i,y2, y2_1, y2_2;
  int x_vertice, y_vertice;
  double offset;
  double dist1;
  double dist2;
  double a,b,c;
  double m, m2, q;
  double t;
  double step;
  double xt,yt;
  double ax,ay;
  double bx,by;
  double cx,cy;
  double dx,dy;
  double fx,fy;
  double b2;
  double x2_1;
  double x2_2;

  // CURVE QUADRATICHE
  if(tot_p==2){
     // calcola i coefficienti
     bx = 2 * (p[1].x - p[0].x);
     ax = p[2].x - p[0].x - bx;

     by = 2 * (p[1].y - p[0].y);
     ay = p[2].y - p[0].y - by;


     // cicla tra 0 e 1 con passo step

     t = 0;
     step = 0.01;
     Sfondo->Canvas->Pen->Color = clYellow;
   //Sfondo->Canvas->Pen->Width = 3;

     Sfondo->Canvas->MoveTo(p[0].x, p[0].y);
     while (t < 1) {
        xt = ax * pow(t, 2) + bx * t + p[0].x;
        yt = ay * pow(t, 2) + by * t + p[0].y;

        x = (int) floor(xt + 0.5);
        y = (int) floor(yt + 0.5);

        Sfondo->Canvas->LineTo(x,y);
        t += step;
     }
     //----------------------------------------------------------------------------------
     // Disegno tangenti di entrata e uscita curva in modialità modifica curve di bezier
     //----------------------------------------------------------------------------------
     Sfondo->Canvas->Pen->Width = 2;
     Sfondo->Canvas->Pen->Color = clRed;
     //------------------------------------------------------
     // Il vertice di controllo è sempre il punto 1
     //------------------------------------------------------
     x_vertice=p[1].x;
     y_vertice=p[1].y;
     //----------------------------------------------------
     // calcolo il coefficiente angolare col punto 0
     //----------------------------------------------------
     //---------------------------------------------------------------------------------
     // se il vertice è in asse x con il punto il coefficente angolare è + o - infinito
     //---------------------------------------------------------------------------------
     if(x_vertice==p[0].x){
        //------------------------------------------------------
        // se il vertice è sopra il coefficente è + infinito
        //------------------------------------------------------
        if(y_vertice<p[0].y) m=+9999;
        //------------------------------------------------------
        // se il vertice è sopra il coefficente è - infinito
        //------------------------------------------------------
        else                 m=-9999;
     }
     //---------------------------------------------------------------------------------
     // se il vertice NON è in asse x con il punto il coefficente lo calcolo
     //---------------------------------------------------------------------------------
     else{
        m=(double)(p[0].y-y_vertice)/(double)(p[0].x-x_vertice);
     }
     q=(double)y_vertice-m*x_vertice;
     //-----------------------------------------------------------------------------
     // Se il coefficente angolare è 0 lo approssimo per evitare divisioni per 0
     //-----------------------------------------------------------------------------
     if(m==0) m=0.0001;
     //-------------------------------------------------------------------------
     // Cerco la distanza tra il vertice è il punto di uscita della curva
     //-------------------------------------------------------------------------
     dist1=sqrt_algo(abs((x_vertice-p[0].x)*(x_vertice-p[0].x))+abs((y_vertice-p[0].y)*(y_vertice-p[0].y)));
     //---------------------------------------------------------------------------------------------------------
     // aggiungo un offset per prolungare la tangente di uscita di una distanza fissa oltre il punto di uscita
     //---------------------------------------------------------------------------------------------------------
     offset= ((200/Rap)*Rid)  +dist1;
     //-------------------------------------------------------------------------------------------------
     // mi ricavo i fattori della eq di 2 grado per trovare il secondo punto della tangente di uscita
     //-------------------------------------------------------------------------------------------------
     a=1+(m*m);
     b=(2*q*m)-(2*y_vertice*m)-(2*x_vertice);
     c=(q*q)+(x_vertice*x_vertice)-(2*q*y_vertice)+(y_vertice*y_vertice)-(offset*offset);
     //----------------------------------------------
     // risolvo l'equazione (mi da due punti)
     //----------------------------------------------
     if(((b*b)-(4*a*c))>=0){ // il delta di risoluzione dell'eq deve essere positivo (condizione di esistenza per poter fare la radice)
        //-------------------------------------------------
        // primo punto:x
        //-------------------------------------------------
        x2_1=((b*(-1))+sqrt_algo((b*b)-(4*a*c)))/(2*a);
        //-------------------------------------------------
        // primo punto:y (dall'equazione della retta)
        //-------------------------------------------------
        y2_1=m*x2_1+q;
        //-------------------------------------------------
        // secondo punto:x
        //-------------------------------------------------
        x2_2=((b*(-1))-sqrt_algo((b*b)-(4*a*c)))/(2*a);
        //-------------------------------------------------
        // secondo punto:y (dall'equazione della retta)
        //-------------------------------------------------
        y2_2=m*x2_2+q;
        //------------------------------------------------------------
        // controllo la distanza dei due punti dal nodo di uscita
        // quello più vicino è quello giusto
        //------------------------------------------------------------
        dist1=sqrt_algo(abs((x2_1-p[0].x)*(x2_1-p[0].x))+abs((y2_1-p[0].y)*(y2_1-p[0].y)));
        dist2=sqrt_algo(abs((x2_2-p[0].x)*(x2_2-p[0].x))+abs((y2_2-p[0].y)*(y2_2-p[0].y)));
        if(dist1<dist2){
           Sfondo->Canvas->MoveTo(x_vertice, y_vertice);
           Sfondo->Canvas->LineTo(x2_1, y2_1);
        }
        else{
           Sfondo->Canvas->MoveTo(x_vertice, y_vertice);
           Sfondo->Canvas->LineTo(x2_2, y2_2);
        }
     }
     //----------------------------------------------------
     // calcolo il coefficiente angolare col punto 2
     //----------------------------------------------------
     //---------------------------------------------------------------------------------
     // se il vertice è in asse x con il punto il coefficente angolare è + o - infinito
     //---------------------------------------------------------------------------------
     if(x_vertice==p[2].x){
        //------------------------------------------------------
        // se il vertice è sopra il coefficente è + infinito
        //------------------------------------------------------
        if(y_vertice<p[2].y) m=+9999;
        //------------------------------------------------------
        // se il vertice è sopra il coefficente è - infinito
        //------------------------------------------------------
        else                 m=-9999;
     }
     //---------------------------------------------------------------------------------
     // se il vertice NON è in asse x con il punto il coefficente lo calcolo
     //---------------------------------------------------------------------------------
     else{
        m=(double)(p[2].y-y_vertice)/(double)(p[2].x-x_vertice);
     }
     q=(double)y_vertice-m*x_vertice;
     if(m==0) m=0.0001;
     //-------------------------------------------------------------------------
     // Cerco la distanza tra il vertice è il punto di uscita della curva
     //-------------------------------------------------------------------------
     dist1=sqrt_algo(abs((x_vertice-p[2].x)*(x_vertice-p[2].x))+abs((y_vertice-p[2].y)*(y_vertice-p[2].y)));
     //---------------------------------------------------------------------------------------------------------
     // aggiungo un offset per prolungare la tangente di uscita di una distanza fissa oltre il punto di uscita
     //---------------------------------------------------------------------------------------------------------
     offset= ((200/Rap)*Rid)  +dist1;
     //-------------------------------------------------------------------------------------------------
     // mi ricavo i fattori della eq di 2 grado per trovare il secondo punto della tangente di uscita
     //-------------------------------------------------------------------------------------------------
     a=1+(m*m);
     b=(2*q*m)-(2*y_vertice*m)-(2*x_vertice);
     c=(q*q)+(x_vertice*x_vertice)-(2*q*y_vertice)+(y_vertice*y_vertice)-(offset*offset);
     //----------------------------------------------
     // risolvo l'equazione (mi da due punti)
     //----------------------------------------------
     if( ((b*b)-(4*a*c))>=0){   // il delta di risoluzione dell'eq deve essere positivo (condizione di esistenza per poter fare la radice)
        //-------------------------------------------------
        // primo punto:x
        //-------------------------------------------------
        x2_1=((b*(-1))+sqrt_algo((b*b)-(4*a*c)))/(2*a);
        //-------------------------------------------------
        // primo punto:y (dall'equazione della retta)
        //-------------------------------------------------
        y2_1=m*x2_1+q;
        //-------------------------------------------------
        // secondo punto:x
        //-------------------------------------------------
        x2_2=((b*(-1))-sqrt_algo((b*b)-(4*a*c)))/(2*a);
        //-------------------------------------------------
        // secondo punto:y (dall'equazione della retta)
        //-------------------------------------------------
        y2_2=m*x2_2+q;
        //------------------------------------------------------------
        // controllo la distanza dei due punti dal nodo di uscita
        // quello più vicino è quello giusto
        //------------------------------------------------------------
        dist1=sqrt_algo(abs((x2_1-p[2].x)*(x2_1-p[2].x))+abs((y2_1-p[2].y)*(y2_1-p[2].y)));
        dist2=sqrt_algo(abs((x2_2-p[2].x)*(x2_2-p[2].x))+abs((y2_2-p[2].y)*(y2_2-p[2].y)));
        if(dist1<dist2){
           Sfondo->Canvas->MoveTo(x_vertice, y_vertice);
           Sfondo->Canvas->LineTo(x2_1, y2_1);
        }
        else{
           Sfondo->Canvas->MoveTo(x_vertice, y_vertice);
           Sfondo->Canvas->LineTo(x2_2, y2_2);
        }
     }
     Sfondo->Canvas->Pen->Width=1; // ritorno dimensione Pen standard
  }
  // CURVE CUBICHE
  if(tot_p==3){
     // calcola i coefficienti
     cx = 3 * (p[1].x - p[0].x);
     bx = 3 * (p[2].x - p[1].x) - cx;
     ax = p[3].x - p[0].x - cx - bx;

     cy = 3 * (p[1].y - p[0].y);
     by = 3 * (p[2].y - p[1].y) - cy;
     ay = p[3].y - p[0].y - cy - by;

     // cicla tra 0 e 1 con passo step
     t = 0;
     step = 0.01;
     Sfondo->Canvas->Pen->Color = clYellow;
     Sfondo->Canvas->MoveTo(p[0].x, p[0].y);

     while (t < 1) {
        xt = ax * pow(t, 3) + bx * pow(t, 2) + cx * t + p[0].x;
        yt = ay * pow(t, 3) + by * pow(t, 2) + cy * t + p[0].y;

        x = (int) floor(xt + 0.5);
        y = (int) floor(yt + 0.5);

        Sfondo->Canvas->LineTo(x,y);
        t += step;
     }
  }
  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       BloccoGrafico()
// ----------------------------------------------
// Funzione che disegna un rettangolo colorato internamente (DARKGRAY)
// con un contorno (BROWN) che ha come coordinate "x1", "y1", "x2", "y2,
// e un eventulae testo da stampare testo
//
void TFormLayout::BloccoGrafico(int X, int Y, int LatoX, int LatoY, TColor ColS, TColor ColB, TColor ColM, char *Testo, int Tipo, int Rot, bool Sel)
/********************/
{
  int i, j, Num;
  float KXb, KYb;
  int XMess, YMess;
  int Xb, Yb, LXb, LYb;
  int X1b, Y1b, X2b, Y2b;
  int Xstart, Ystart, Xstop, Ystop;
  int xcen, ycen, offs;
  int X1pix, Y1pix, X2pix, Y2pix, X3pix, Y3pix, X4pix, Y4pix;
  long double sinB1, cosB1, sinB2, cosB2;
  long double grd, radA, radB, radB1, radB2;
  long double ComponenteX1, ComponenteX2, ComponenteY1, ComponenteY2;
  long double MezzoLatoX, MezzoLatoY, MezzaDiagonale;
  TPoint points[5];

  //------------------------------------
  // Assegna i colori al Blocco
  //------------------------------------
  Sfondo->Canvas->Pen->Color         = ColB;
  Sfondo->Canvas->Pen->Style         = psSolid;
  if(Sel) Sfondo->Canvas->Pen->Color = (TColor)(0x00FF80FF);
  Sfondo->Canvas->Brush->Style       = bsSolid;
  if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
  Sfondo->Canvas->Brush->Color       = ColS;
  Sfondo->Canvas->Font->Color        = ColM;
  //------------------------------------
  // Coordinate del blocco
  //------------------------------------
  Xstart = X;
  Ystart = Y;
  Xstop  = X+LatoX;
  Ystop  = Y+LatoY;

  //=======================================================================
  // GRADI DI INCLINAZIONE
  // Solo per rettangoli con rotazione diverse da "0" e "1"
  //=======================================================================
  if(Rot!=0 && Rot!=1 && Tipo==0 && ((LatoX<-1 || LatoX>1) && (LatoY<-1 || LatoY>1))){
     //----------------------------------------------------
     // Con calcolo Angolo orientamento
     //----------------------------------------------------
     grd  = (long double)(Rot);
     radA = DegToRad(grd);

     MezzoLatoX = (long double)(LatoX/2);
     MezzoLatoY = (long double)(LatoY/2);


     xcen = (((X+MezzoLatoX)-offset[1][0]));        // pixel x del nodo occupato
     ycen = (((Y+MezzoLatoY)-offset[1][1]));        // pixel y del nodo occupato


     radB  = (long double)(atanl( MezzoLatoY / MezzoLatoX ));
     radB1 = radA + radB;
     radB2 = radA - radB;
     SinCos(radB1, sinB1, cosB1);
     SinCos(radB2, sinB2, cosB2);

     MezzaDiagonale = (long double)(sqrt((MezzoLatoX*MezzoLatoX)+(MezzoLatoY*MezzoLatoY)));

     ComponenteX1 = (long double) ( MezzaDiagonale*cosB1 );
     ComponenteX2 = (long double) ( MezzaDiagonale*cosB2 );
     ComponenteY1 = (long double) ( MezzaDiagonale*sinB1 );
     ComponenteY2 = (long double) ( MezzaDiagonale*sinB2 );

     X1pix = (int) (xcen-(int)(ComponenteX1));
     X2pix = (int) (xcen+(int)(ComponenteX2));
     X3pix = (int) (xcen+(int)(ComponenteX1));
     X4pix = (int) (xcen-(int)(ComponenteX2));

     Y1pix = (int) (ycen+(int)(ComponenteY1));
     Y2pix = (int) (ycen-(int)(ComponenteY2));
     Y3pix = (int) (ycen-(int)(ComponenteY1));
     Y4pix = (int) (ycen+(int)(ComponenteY2));

     //-----------------------------------------------
     // Aggiustamenti dovuti ad arrotondamenti
     //-----------------------------------------------
     if(grd==90 || grd==270){
        offs = abs(X4pix-X3pix);
        if(offs<3) X4pix=X3pix;
        offs = abs(X2pix-X1pix);
        if(offs<3) X2pix=X1pix;
        offs = abs(Y4pix-Y1pix);
        if(offs<3) Y4pix=Y1pix;
        offs = abs(Y3pix-Y2pix);
        if(offs<3) Y3pix=Y2pix;
     }

     if(grd==0 || grd==180){
        offs = abs(X4pix-X1pix);
        if(offs<3) X4pix=X1pix;
        offs = abs(X3pix-X2pix);
        if(offs<3) X3pix=X2pix;
        offs = abs(Y2pix-Y1pix);
        if(offs<3) Y2pix=Y1pix;
        offs = abs(Y4pix-Y3pix);
        if(offs<3) Y4pix=Y3pix;
     }

     //-----------------------------------------------
     // Coordinate dei punti su cui deve essere
     // costruito il rettangolo di sfondo
     //-----------------------------------------------
     points[0] = Point(X1pix/Rap*Rid, Y1pix/Rap*Rid);
     points[1] = Point(X2pix/Rap*Rid, Y2pix/Rap*Rid);
     points[2] = Point(X3pix/Rap*Rid, Y3pix/Rap*Rid);
     points[3] = Point(X4pix/Rap*Rid, Y4pix/Rap*Rid);
     //------------------------------------
     // Disegno grafico
     //------------------------------------
     Sfondo->Canvas->Polygon(points, 3);
  }
  //=======================================================================
  // VECCHIA GESTIONE SFONDO GRAFICO
  // - Rotazione "0", "1" per compatibilità con impianti già in funzione
  // - Ellissi
  // - Linee
  //=======================================================================
  else{
     //------------------------------------
     // converto le coordinate in pixel
     //------------------------------------
     X1pix =(int)(((Xstart-offset[1][0])/Rap)*Rid);
     Y1pix =(int)(((Ystart-offset[1][1])/Rap)*Rid);
     X2pix =(int)(((Xstop-offset[1][0])/Rap)*Rid);
     Y2pix =(int)(((Ystop-offset[1][1])/Rap)*Rid);
     //------------------------------------
     // Disegno grafico
     //------------------------------------
     switch(Tipo){
        case  0: Sfondo->Canvas->Rectangle(X1pix, Y1pix, X2pix, Y2pix);
                 break;
        case  1: Sfondo->Canvas->Ellipse(X1pix, Y1pix, X2pix, Y2pix);
                 break;
        default: Num = Tipo-1;
                 if(Sel){
                    Sfondo->Canvas->Pen->Style=psDot;
                    Sfondo->Canvas->Rectangle(X1pix, Y1pix, X2pix, Y2pix);
                 }
                 //--------------------------------
                 // Disegno del blocco speciale
                 //--------------------------------
                 for(i=1; i<=MAXBLOCCHISEMPLICI; i++){
                    if(!BloccoS[Num].Blocco[i].LatoX && !BloccoS[Num].Blocco[i].LatoY) break;
                    Xb  = BloccoS[Num].Blocco[i].X;
                    Yb  = BloccoS[Num].Blocco[i].Y;
                    LXb = BloccoS[Num].Blocco[i].LatoX;
                    LYb = BloccoS[Num].Blocco[i].LatoY;
                    KXb = (float)(LatoX)/(float)(260);
                    KYb = (float)(LatoY)/(float)(260);
                    //------------------------------------
                    // Rotazione
                    //------------------------------------
                    if(Rot){
                       j   = LXb;           // appoggio
                       LXb = LYb;
                       LYb = j;
                       X1b = (Yb*KXb)+Xstart;
                       Y1b = (Xb*KYb)+Ystart;
                    }
                    else{
                       X1b = (Xb*KXb)+Xstart;
                       Y1b = (Yb*KYb)+Ystart;
                    }
                    X2b = (X1b+(LXb*KXb));
                    Y2b = (Y1b+(LYb*KYb));
                    X1pix =(int)(((X1b-offset[1][0])/Rap)*Rid);
                    Y1pix =(int)(((Y1b-offset[1][1])/Rap)*Rid);
                    X2pix =(int)(((X2b-offset[1][0])/Rap)*Rid);
                    Y2pix =(int)(((Y2b-offset[1][1])/Rap)*Rid);
                    Sfondo->Canvas->Pen->Style   = psSolid;
                    Sfondo->Canvas->Pen->Color   = (TColor)(BloccoS[Num].Blocco[i].ColB);
                    if(Sel) Sfondo->Canvas->Pen->Color = (TColor)(0x00FF80FF);
                    Sfondo->Canvas->Brush->Style = bsSolid;
                    if(VisualizzaZoneImpianto) Sfondo->Canvas->Brush->Style = bsClear;
                    Sfondo->Canvas->Brush->Color = (TColor)(BloccoS[Num].Blocco[i].ColS);
                    switch(BloccoS[Num].Blocco[i].Tipo){
                       case  0: Sfondo->Canvas->Rectangle(X1pix, Y1pix, X2pix, Y2pix);
                                break;
                       case  1: Sfondo->Canvas->Ellipse(X1pix, Y1pix, X2pix, Y2pix);
                                break;
                       default: Sfondo->Canvas->Rectangle(X1pix, Y1pix, X2pix, Y2pix);
                                break;
                    }
                 }
                 break;
     }
     //------------------------------------
     // Disegna Linea
     //------------------------------------
     if(Tipo<2 && (Xstart==Xstop || Ystart==Ystop)){
        Sfondo->Canvas->MoveTo( X1pix, Y1pix );
        Sfondo->Canvas->LineTo( X2pix, Y2pix );
     }
  }
  //------------------------------------
  // Scrittura del testo all'interno
  // del blocco
  //------------------------------------
  XMess = (X+(LatoX/2))-((strlen(Testo)/2)*60);
  YMess = (Y+(LatoY/2))-60;
  XMess =(int)(((XMess-offset[1][0])/Rap)*Rid);
  YMess =(int)(((YMess-offset[1][1])/Rap)*Rid);
  if(Rid>=RID_NUMERI) Sfondo->Canvas->TextOut(XMess, YMess, Testo);
}
//---------------------------------------------------------------------------
//-------------------------------------------------------
//      SfondoGrafico()
//-------------------------------------------------------
// Questa funzione esegue tutte quelle parti di disegno statico (sfondo)
// che sono tipiche di ogni impianto ( muri, ingombri, sagome di altre
// macchine, separazioni ).
// ( Eventuali  misure sempre in Cm ! )
//
// -------> Il Prototipo è definito in MAIN.H
//
void TFormLayout::SfondoGrafico()
/*****************************************/
{
  int err=0;
  int i, len;
  int handle;
  char Testo[101]="";
  char filename[101]="SFONDO.DAT";
  bool Sel;
  TColor ColS, ColB, ColM;
  int X, Y, LX, LY, Tipo, Rot;
  static bool Rinfresco=true;

  //--------------------------------------------------------
  // Se il file con lo stato dello Sfondo Grafico è stato
  // aggiornato lo rileggo.
  //--------------------------------------------------------
  if(!access("AGGIORNA.DAT", 0) || Rinfresco){
     Rinfresco=false;
     unlink("AGGIORNA.DAT");
     //--------------------------------------------------------
     // Abilitazioni/Disabilitazioni Impianto di default
     //--------------------------------------------------------
     memset(&BloccoG, 0, sizeof(BloccoG));
     memset(&BloccoS, 0, sizeof(BloccoS));
     //--------------------------------------------------------
     // Compilazione nome file degli SFONDO GRAFICO
     //--------------------------------------------------------
     strcpy(filename, "GRAFICA\\SFONDO.DAT");
     //--------------------------------------------------------
     // Apertura file SFONDO.DAT
     //--------------------------------------------------------
     handle = _rtl_open(filename, O_RDWR);
     if(handle<0){
        _rtl_close( handle );
        return;
     }
     //--------------------------------------------------------
     // Lettura file SFONDO.DAT
     //--------------------------------------------------------
     len = _rtl_read( handle, &BloccoG, sizeof(BloccoG));
     if(len != sizeof(BloccoG)) err=1;
     if(err) memset(&BloccoG, 0, sizeof(BloccoG));
     _rtl_close( handle );
     //--------------------------------------------------------
     // Rilettura della struttura dei blocchi speciali
     //--------------------------------------------------------
     for(i=0; i<=MAXBLOCCHISPECIALI; i++){
        sprintf(filename, "GRAFICA\\BLOCCO%02d.DAT", i);
        //--------------------------------------------------------
        // Apertura file BLOCCOxx.DAT
        //--------------------------------------------------------
        handle = _rtl_open(filename, O_RDWR);
        if(handle<0){
           _rtl_close( handle );
           continue;
        }
        //--------------------------------------------------------
        // Lettura file BLOCCOxx.DAT
        //--------------------------------------------------------
        len = _rtl_read( handle, &BloccoS[i], sizeof(BLOCCOSPECIALE));
        if(len != sizeof(BLOCCOSPECIALE)) err=1;
        if(err) memset(&BloccoS[i], 0, sizeof(BLOCCOSPECIALE));
        _rtl_close( handle );
     }
  }
  //--------------------------------------------------------
  // Disegno dei Blocchi Grafici
  //--------------------------------------------------------
  for(i=1; i<=MAXBLOCCHIGRAFICI; i++){
     if(!BloccoG[i].X && !BloccoG[i].Y) break;
     //----------------------------------------------
     // Coordinate e rotazione
     //----------------------------------------------
     X    = BloccoG[i].X;
     Y    = BloccoG[i].Y;
     LX   = BloccoG[i].LatoX;
     LY   = BloccoG[i].LatoY;
     Tipo = BloccoG[i].Tipo;
     Rot  = BloccoG[i].Rotazione;
     //----------------------------------------------
     // Colori
     //----------------------------------------------
     ColB = (TColor)(BloccoG[i].ColPen);
     ColS = (TColor)(BloccoG[i].ColBrush);
     ColM = (TColor)(BloccoG[i].ColStringa);
     //----------------------------------------------
     // Verifico se il blocco è quello selezionato
     //----------------------------------------------
     Sel=false;
     if(BloccoG[i].Selezionato=='Y' && AbilitaSfondoGrafico) Sel=true;
     //----------------------------------------------
     // Recupero la stringa da scrivere internamente
     // al blocco selezionato
     //----------------------------------------------
     memcpy(&Testo[0], &BloccoG[i].Stringa[0], sizeof(BloccoG[i].Stringa));
     //--------------------------------------------
     // Disegno il blocco selezionato
     //--------------------------------------------
     BloccoGrafico(X, Y, LX, LY, ColS, ColB, ColM, Testo, Tipo, Rot, Sel);
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       StampaStringa()
// ----------------------------------------------
void TFormLayout::StampaStringa( Graphics::TBitmap *Bmp, int x, int y, char *stringa, TColor col)
/*******************************************************************************/
{
  int i;

  Bmp->Canvas->Pen->Color = col;
  for(i=0;;i++){
     if( stringa[i]==0 )break;
     if(( stringa[i] >= '0') && (stringa[i] <= '9')){
        switch( stringa[i] ){
           case '0': Bmp->Canvas->MoveTo( x,y );
                     Bmp->Canvas->LineTo( x+2,y);
                     Bmp->Canvas->LineTo( x+2,y+4);
                     Bmp->Canvas->LineTo( x,y+4);
                     Bmp->Canvas->LineTo( x,y);
                     break;
           case '1': Bmp->Canvas->MoveTo( x+1,y  );
                     Bmp->Canvas->LineTo( x+1,y+5);
                     Bmp->Canvas->MoveTo(   x,y+4);
                     Bmp->Canvas->LineTo( x+3,y+4);
                     Bmp->Canvas->MoveTo(   x,y+1);
                     Bmp->Canvas->LineTo( x+2,y+1);
                     break;
           case '2': Bmp->Canvas->MoveTo(  x,y );
                     Bmp->Canvas->LineTo( x+2,y);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     Bmp->Canvas->LineTo( x,y+2);
                     Bmp->Canvas->LineTo( x,y+4);
                     Bmp->Canvas->LineTo( x+3,y+4);
                     break;
           case '3': Bmp->Canvas->MoveTo(  x,y );
                     Bmp->Canvas->LineTo( x+2,y);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     Bmp->Canvas->LineTo(   x,y+2);
                     Bmp->Canvas->MoveTo( x+2,y+2);
                     Bmp->Canvas->LineTo( x+2,y+4);
                     Bmp->Canvas->LineTo( x-1,y+4);
                     break;
           case '4': Bmp->Canvas->MoveTo(  x,y );
                     Bmp->Canvas->LineTo( x,y+2);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     Bmp->Canvas->MoveTo( x+2,y);
                     Bmp->Canvas->LineTo( x+2,y+5);
                     break;
           case '5': Bmp->Canvas->MoveTo(  x+2,y );
                     Bmp->Canvas->LineTo( x,y);
                     Bmp->Canvas->LineTo( x,y+2);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     Bmp->Canvas->LineTo( x+2,y+4);
                     Bmp->Canvas->LineTo( x-1,y+4);
                     break;
           case '6': Bmp->Canvas->MoveTo(  x+2,y );
                     Bmp->Canvas->LineTo( x,y);
                     Bmp->Canvas->LineTo( x,y+4);
                     Bmp->Canvas->LineTo( x,y+2);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     Bmp->Canvas->LineTo( x+2,y+4);
                     Bmp->Canvas->LineTo( x,y+4);
                     break;
           case '7': Bmp->Canvas->MoveTo(  x,y );
                     Bmp->Canvas->LineTo( x+2,y);
                     Bmp->Canvas->LineTo( x+2,y+5);
                     break;
           case '8': Bmp->Canvas->MoveTo(  x,y );
                     Bmp->Canvas->LineTo( x+2,y);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     Bmp->Canvas->LineTo( x,y+2);
                     Bmp->Canvas->LineTo( x,y);
                     Bmp->Canvas->LineTo( x,y+4);
                     Bmp->Canvas->LineTo( x+2,y+4);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     break;
           case '9': Bmp->Canvas->MoveTo(  x,y+4 );
                     Bmp->Canvas->LineTo( x+2,y+4);
                     Bmp->Canvas->LineTo( x+2,y);
                     Bmp->Canvas->LineTo( x,y);
                     Bmp->Canvas->LineTo( x,y+2);
                     Bmp->Canvas->LineTo( x+2,y+2);
                     break;
        } //end switch
     } // endif
     else{
        switch( stringa[i] ){
           case 'A': Bmp->Canvas->MoveTo(x, y+4);
                     Bmp->Canvas->LineTo(x, y+1);
                     Bmp->Canvas->LineTo(x+1,y);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+3,y+5);
                     Bmp->Canvas->MoveTo(x,y+2);
                     Bmp->Canvas->LineTo(x+4,y+2);
                     break;
           case 'B': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+2,y+2);
                     Bmp->Canvas->LineTo(x-1,y+2);
                     Bmp->Canvas->MoveTo(x+2,y+2);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x-1,y);
                     break;
           case 'C': Bmp->Canvas->MoveTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2, y);
                     Bmp->Canvas->LineTo(x+1, y);
                     Bmp->Canvas->LineTo(x,y+1);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+4,y+3);
                     break;
           case 'D': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x, y+4);
                     Bmp->Canvas->LineTo(x, y);
                     break;
           case 'E': Bmp->Canvas->MoveTo(x+3,y);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x,y+4);
                     Bmp->Canvas->LineTo(x+4,y+4);
                     Bmp->Canvas->MoveTo(x, y+2);
                     Bmp->Canvas->LineTo(x+3,y+2);
                     break;
           case 'F': Bmp->Canvas->MoveTo(x+3,y);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x,y+5);
                     Bmp->Canvas->MoveTo(x,y+2);
                     Bmp->Canvas->LineTo(x+3,y+2);
                     break;
           case 'G': Bmp->Canvas->MoveTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+1,y);
                     Bmp->Canvas->LineTo(x,y+1);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+1,y+3);
                     break;
           case 'H': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+5);
                     Bmp->Canvas->MoveTo(x+3,y);
                     Bmp->Canvas->LineTo(x+3,y+5);
                     Bmp->Canvas->MoveTo(x,y+2);
                     Bmp->Canvas->LineTo(x+4,y+2);
                     break;
           case 'I': x++;
                     Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x, y+5);
                     x-=2;
                     break;
           case 'J': Bmp->Canvas->MoveTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+3,y-1);
                     break;
           case 'K': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+5);
                     Bmp->Canvas->MoveTo(x+2,y);
                     Bmp->Canvas->LineTo(x+2,y+1);
                     Bmp->Canvas->LineTo(x+1,y+2);
                     Bmp->Canvas->LineTo(x+2,y+3);
                     Bmp->Canvas->LineTo(x+2,y+5);
                     x--;
                     break;
           case 'L': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+4);
                     Bmp->Canvas->LineTo(x+4,y+4);
                     break;
           case 'M': Bmp->Canvas->MoveTo(x,y+4);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x+2,y+2);
                     Bmp->Canvas->LineTo(x+4,y);
                     Bmp->Canvas->LineTo(x+4,y+5);
                     x++;
                     break;
           case 'N': Bmp->Canvas->MoveTo(x,y+4);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->MoveTo(x+3,y);
                     Bmp->Canvas->LineTo(x+3,y+5);
                     break;
           case 'O': Bmp->Canvas->MoveTo(x,y+1);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+1,y);
                     Bmp->Canvas->LineTo(x,y+1);
                     break;
           case 'P': Bmp->Canvas->MoveTo(x, y+4);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2,y+2);
                     Bmp->Canvas->LineTo(x-1,y+2);
                     break;
           case 'Q': Bmp->Canvas->MoveTo(x,y+1);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+1,y);
                     Bmp->Canvas->LineTo(x,y+1);
                     Bmp->Canvas->MoveTo(x+2,y+3);
                     Bmp->Canvas->LineTo(x+3,y+5);
                     break;
           case 'R': Bmp->Canvas->MoveTo(x, y+4);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x+2,y+2);
                     Bmp->Canvas->LineTo(x,y+2);
                     Bmp->Canvas->LineTo(x+3,y+5);
                     break;
           case 'S': Bmp->Canvas->MoveTo( x+3,y );
                     Bmp->Canvas->LineTo(x+1,y);
                     Bmp->Canvas->LineTo(x,y+1);
                     Bmp->Canvas->LineTo(x+1,y+2);
                     Bmp->Canvas->LineTo(x+2,y+2);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x-1,y+4);
                     break;
           case 'T': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x+5,y);
                     Bmp->Canvas->MoveTo(x+2,y);
                     Bmp->Canvas->LineTo(x+2,y+5);
                     x++;
                     break;
           case 'U': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+3,y-1);
                     break;
           case 'V': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+2);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x+4,y+2);
                     Bmp->Canvas->LineTo(x+4,y-1);
                     x++;
                     break;
           case 'W': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x+1,y+4);
                     Bmp->Canvas->LineTo(x+2,y+3);
                     Bmp->Canvas->LineTo(x+3,y+4);
                     Bmp->Canvas->LineTo(x+4,y+3);
                     Bmp->Canvas->LineTo(x+4,y-1);
                     x++;
                     break;
           case 'X': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+1);
                     Bmp->Canvas->LineTo(x+3,y+3);
                     Bmp->Canvas->LineTo(x+3,y+5);
                     Bmp->Canvas->MoveTo(x+3,y);
                     Bmp->Canvas->LineTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x,y+3);
                     Bmp->Canvas->LineTo(x,y+5);
                     break;
           case 'Y': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x,y+1);
                     Bmp->Canvas->LineTo(x+1,y+2);
                     Bmp->Canvas->LineTo(x+2,y+1);
                     Bmp->Canvas->LineTo(x+2,y-1);
                     Bmp->Canvas->MoveTo(x+1,y+2);
                     Bmp->Canvas->LineTo(x+1,y+5);
                     x--;
                     break;
           case 'Z': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x+4,y);
                     Bmp->Canvas->MoveTo(x+3,y+1);
                     Bmp->Canvas->LineTo(x,y+4);
                     Bmp->Canvas->LineTo(x+4,y+4);
                     break;
           case '[': Bmp->Canvas->MoveTo(x+2,y);
                     Bmp->Canvas->LineTo(x,y);
                     Bmp->Canvas->LineTo(x,y+4);
                     Bmp->Canvas->LineTo(x+3,y+4);
                     x--;
                     break;
           case ']': Bmp->Canvas->MoveTo(x,y);
                     Bmp->Canvas->LineTo(x+2,y);
                     Bmp->Canvas->LineTo(x+2,y+4);
                     Bmp->Canvas->LineTo(x-1,y+4);
                     break;
           case '-': Bmp->Canvas->MoveTo(x,y+2);
                     Bmp->Canvas->LineTo(x+4,y+2);
        }
     }
     x+=5;
  }
}
//---------------------------------------------------------------------------
// ------------------------
//    StampNum()
// ------------------------
void TFormLayout::StampNum( Graphics::TBitmap *Bmp, int x, int y, char *num, TColor col, bool Little, int Down)
/***********************************************************************************************/
{
  int i;

  Bmp->Canvas->Pen->Color = col;

  if((Rid>=RID_NUMERI && Rid<0.8) || Little==true){
     if(Down==0) y=y-6;
     if(Down==1) y=y+2;
     if( strlen(num)==1 )x=x-4;
     if( strlen(num)==2 )x=x-8;
     if( strlen(num)==3 )x=x-12;
     if( strlen(num)==4 )x=x-16;
     for(i=0;;i++){
        if( num[i]==0 )break;
        switch( num[i] ){
           case '0'    :   Bmp->Canvas->MoveTo( x,y );
                           Bmp->Canvas->LineTo( x+2,y);
                           Bmp->Canvas->LineTo( x+2,y+4);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x,y);
                           break;
           case '1'    :   Bmp->Canvas->MoveTo( x+1,y );
                           Bmp->Canvas->LineTo( x+1,y+5);
                           Bmp->Canvas->MoveTo(   x,y+4);
                           Bmp->Canvas->LineTo( x+3,y+4);
                           Bmp->Canvas->MoveTo(   x,y+1);
                           Bmp->Canvas->LineTo( x+2,y+1);
                           break;
           case '2'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+2,y);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           Bmp->Canvas->LineTo( x,y+2);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x+3,y+4);
                           break;
           case '3'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+2,y);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           Bmp->Canvas->LineTo(   x,y+2);
                           Bmp->Canvas->MoveTo( x+2,y+2);
                           Bmp->Canvas->LineTo( x+2,y+4);
                           Bmp->Canvas->LineTo( x-1,y+4);
                           break;
           case '4'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x,y+2);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           Bmp->Canvas->MoveTo( x+2,y);
                           Bmp->Canvas->LineTo( x+2,y+5);
                           break;
           case '5'    :   Bmp->Canvas->MoveTo(  x+2,y );
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+2);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           Bmp->Canvas->LineTo( x+2,y+4);
                           Bmp->Canvas->LineTo( x-1,y+4);
                           break;
           case '6'    :   Bmp->Canvas->MoveTo(  x+2,y );
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x,y+2);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           Bmp->Canvas->LineTo( x+2,y+4);
                           Bmp->Canvas->LineTo( x,y+4);
                           break;
           case '7'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+2,y);
                           Bmp->Canvas->LineTo( x+2,y+5);
                           break;
           case '8'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+2,y);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           Bmp->Canvas->LineTo( x,y+2);
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x+2,y+4);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           break;
           case '9'    :   Bmp->Canvas->MoveTo(  x,y+4 );
                           Bmp->Canvas->LineTo( x+2,y+4);
                           Bmp->Canvas->LineTo( x+2,y);
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+2);
                           Bmp->Canvas->LineTo( x+2,y+2);
                           break;

        }
        x=x+4;
     }
  }
  if(Rid>=0.8 && Rid<1.4 && Little==false){
     if(Down==0) y=y-8;
     if(Down==1) y=y+4;
     if( strlen(num)==1 )x=x-6;
     if( strlen(num)==2 )x=x-12;
     if( strlen(num)==3 )x=x-18;
     if( strlen(num)==4 )x=x-24;
     for(i=0;;i++){
        if( num[i]==0 )break;
        switch( num[i] ){
           case '0'    :   Bmp->Canvas->MoveTo( x,y );
                           Bmp->Canvas->LineTo( x+4,y);
                           Bmp->Canvas->LineTo( x+4,y+6);
                           Bmp->Canvas->LineTo( x,y+6);
                           Bmp->Canvas->LineTo( x,y);
                           break;
           case '1'    :   Bmp->Canvas->MoveTo( x+3,y );
                           Bmp->Canvas->LineTo( x+3,y+7);
                           Bmp->Canvas->MoveTo(   x,y+6);
                           Bmp->Canvas->LineTo( x+5,y+6);
                           Bmp->Canvas->MoveTo(   x,y+2);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           break;
           case '2'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+4,y);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           Bmp->Canvas->LineTo( x,y+3);
                           Bmp->Canvas->LineTo( x,y+6);
                           Bmp->Canvas->LineTo( x+5,y+6);
                           break;
           case '3'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+4,y);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           Bmp->Canvas->LineTo(   x,y+3);
                           Bmp->Canvas->MoveTo( x+4,y+3);
                           Bmp->Canvas->LineTo( x+4,y+6);
                           Bmp->Canvas->LineTo( x-1,y+6);
                           break;
           case '4'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x,y+3);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           Bmp->Canvas->MoveTo( x+4,y);
                           Bmp->Canvas->LineTo( x+4,y+7);
                           break;
           case '5'    :   Bmp->Canvas->MoveTo(  x+4,y );
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+3);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           Bmp->Canvas->LineTo( x+4,y+6);
                           Bmp->Canvas->LineTo( x-1,y+6);
                           break;
           case '6'    :   Bmp->Canvas->MoveTo(  x+4,y );
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+6);
                           Bmp->Canvas->LineTo( x,y+3);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           Bmp->Canvas->LineTo( x+4,y+6);
                           Bmp->Canvas->LineTo( x,y+6);
                           break;
           case '7'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+4,y);
                           Bmp->Canvas->LineTo( x+4,y+7);
                           break;
           case '8'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+4,y);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           Bmp->Canvas->LineTo( x,y+3);
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+6);
                           Bmp->Canvas->LineTo( x+4,y+6);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           break;
           case '9'    :   Bmp->Canvas->MoveTo(  x,y+6 );
                           Bmp->Canvas->LineTo( x+4,y+6);
                           Bmp->Canvas->LineTo( x+4,y);
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+3);
                           Bmp->Canvas->LineTo( x+4,y+3);
                           break;

        }
        x=x+6;
     }
  }
  if(Rid>=1.4 && Little==false){
     if(Down==0) y=y-10;
     if(Down==1) y=y+ 6;
     if( strlen(num)==1 )x=x-8;
     if( strlen(num)==2 )x=x-16;
     if( strlen(num)==3 )x=x-24;
     if( strlen(num)==4 )x=x-32;
     for(i=0;;i++){
        if( num[i]==0 )break;
        switch( num[i] ){
           case '0'    :   Bmp->Canvas->MoveTo( x,y );
                           Bmp->Canvas->LineTo( x+6,y);
                           Bmp->Canvas->LineTo( x+6,y+8);
                           Bmp->Canvas->LineTo( x,y+8);
                           Bmp->Canvas->LineTo( x,y);
                           break;
           case '1'    :   Bmp->Canvas->MoveTo( x+5,y );
                           Bmp->Canvas->LineTo( x+5,y+9);
                           Bmp->Canvas->MoveTo(   x,y+8);
                           Bmp->Canvas->LineTo( x+7,y+8);
                           Bmp->Canvas->MoveTo(   x,y+3);
                           Bmp->Canvas->LineTo( x+5,y  );
                           break;
           case '2'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+6,y);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x,y+8);
                           Bmp->Canvas->LineTo( x+7,y+8);
                           break;
           case '3'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+6,y);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           Bmp->Canvas->LineTo(   x,y+4);
                           Bmp->Canvas->MoveTo( x+6,y+4);
                           Bmp->Canvas->LineTo( x+6,y+8);
                           Bmp->Canvas->LineTo( x-1,y+8);
                           break;
           case '4'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           Bmp->Canvas->MoveTo( x+6,y);
                           Bmp->Canvas->LineTo( x+6,y+9);
                           break;
           case '5'    :   Bmp->Canvas->MoveTo(  x+6,y );
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           Bmp->Canvas->LineTo( x+6,y+8);
                           Bmp->Canvas->LineTo( x-1,y+8);
                           break;
           case '6'    :   Bmp->Canvas->MoveTo(  x+6,y );
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+8);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           Bmp->Canvas->LineTo( x+6,y+8);
                           Bmp->Canvas->LineTo( x,y+8);
                           break;
           case '7'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+6,y);
                           Bmp->Canvas->LineTo( x+6,y+9);
                           break;
           case '8'    :   Bmp->Canvas->MoveTo(  x,y );
                           Bmp->Canvas->LineTo( x+6,y);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+8);
                           Bmp->Canvas->LineTo( x+6,y+8);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           break;
           case '9'    :   Bmp->Canvas->MoveTo(  x,y+8 );
                           Bmp->Canvas->LineTo( x+6,y+8);
                           Bmp->Canvas->LineTo( x+6,y);
                           Bmp->Canvas->LineTo( x,y);
                           Bmp->Canvas->LineTo( x,y+4);
                           Bmp->Canvas->LineTo( x+6,y+4);
                           break;

        }
        x=x+8;
     }
  }
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_freccia()
// ----------------------------------------------
void TFormLayout::disegna_freccia(int PosX, int PosY, int largh, int Lungh, int gradi)
{
  int posX, posY, lungh;
  long X, Y, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
  long double rad=0, rad2, sin, cos;

  TPoint points[7];
  TPoint points_ruot[7];
  posX=PosX -offset[1][0];
  posY=PosY -offset[1][1];
  lungh=Lungh;

  Sfondo->Canvas->Pen->Color = clWhite;
  Sfondo->Canvas->Ellipse(posX/Rap*Rid-2, posY/Rap*Rid-2, posX/Rap*Rid+2, posY/Rap*Rid+2);

  rad = DegToRad(gradi);
  SinCos(rad, sin, cos);

  //PUNTO 1
  //-----------------------------------------------
  // Disegno poligono postazione singola
  //-----------------------------------------------
  X = (posX+(largh/4));
  Y = posY;

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid, Y/Rap*Rid, X/Rap*Rid+5, Y/Rap*Rid+5);

  points[0] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;




  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2 , Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[0] = Point(X4/Rap*Rid, Y4/Rap*Rid);


  //PUNTO 2
  X = (posX+(largh/4));
  Y = (posY+(lungh/2));

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid-2, Y/Rap*Rid-2, X/Rap*Rid+2, Y/Rap*Rid+2);

  points[1] = Point(X/Rap*Rid, Y/Rap*Rid);
  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------

  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;

  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[1] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 3

  X = posX;
  Y = (posY+(lungh/2));

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid-2, Y/Rap*Rid-2, X/Rap*Rid+2, Y/Rap*Rid+2);

  points[2] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------

  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;

  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[2] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 4

  X = (posX+(largh/2));
  Y = (posY+lungh);

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid-2, Y/Rap*Rid-2, X/Rap*Rid+2, Y/Rap*Rid+2);

  points[3] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------

  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;

  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[3] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 5

  X = (posX+largh);
  Y = (posY+(lungh/2));

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid-2, Y/Rap*Rid-2, X/Rap*Rid+2, Y/Rap*Rid+2);

  points[4] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------

  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;

  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[4] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 6

  X = (posX+(largh/4)*3);
  Y = (posY+(lungh/2));

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid-2, Y/Rap*Rid-2, X/Rap*Rid+2, Y/Rap*Rid+2);

  points[5] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------

  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;

  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[5] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 7

  X = (posX+(largh/4)*3);
  Y = posY;

  Sfondo->Canvas->Pen->Color   = clRed;
  Sfondo->Canvas->Ellipse(X/Rap*Rid-2, Y/Rap*Rid-2, X/Rap*Rid+2, Y/Rap*Rid+2);

  points[6] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------

  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);

  X4 = X3+posX;
  Y4= Y3+posY;

  Sfondo->Canvas->Pen->Color   = clLime;
  Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);

  points_ruot[6] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  Sfondo->Canvas->Pen->Color   = clWhite;
  //-----------------------------------------------
  // Lampeggio nero.
  //-----------------------------------------------
  Sfondo->Canvas->Brush->Color = clSFONDO;
  //-----------------------------------------------
  // visualizzazione trasparente
  //-----------------------------------------------
  Sfondo->Canvas->Brush->Style = bsClear;

  //-----------------------------------------------
  // Rettangolo
  //-----------------------------------------------
  Sfondo->Canvas->Polygon(points, 6);

  //-----------------------------------------------
  // Rettangolo
  //-----------------------------------------------
  Sfondo->Canvas->Polygon(points_ruot, 6);

  return;
}
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_freccia2()
// ----------------------------------------------
void TFormLayout::disegna_freccia2(int PosX, int PosY, int largh, int lungh, int gradi, TColor col)
{
  int posX, posY;
  long X, Y, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
  long double rad=0, rad2, sin, cos;
  TPoint points[7];
  TPoint points_ruot[7];

  posX=PosX -offset[1][0];
  posY=PosY -offset[1][1];

  rad = DegToRad(gradi);
  SinCos(rad, sin, cos);

  //PUNTO 1
  //-----------------------------------------------
  // Disegno poligono postazione singola
  //-----------------------------------------------
  X = (posX+(largh/4))  - (largh/2);
  Y = posY              - (lungh/2);

  points[0] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  points_ruot[0] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 2
  X = (posX+(largh/4)) - (largh/2);
  Y = (posY+(lungh/2)) - (lungh/2);

  points[1] = Point(X/Rap*Rid, Y/Rap*Rid);
  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  //Sfondo->Canvas->Pen->Color   = clLime;
  //Sfondo->Canvas->Ellipse(X4/Rap*Rid-2, Y4/Rap*Rid-2, X4/Rap*Rid+2, Y4/Rap*Rid+2);
  points_ruot[1] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 3

  X = posX             - (largh/2);
  Y = (posY+(lungh/2)) - (lungh/2);

  points[2] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  points_ruot[2] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 4

  X = (posX+(largh/2)) - (largh/2);
  Y = (posY+lungh)     - (lungh/2);

  points[3] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  points_ruot[3] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 5

  X = (posX+largh)     - (largh/2);
  Y = (posY+(lungh/2)) - (lungh/2);

  points[4] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  points_ruot[4] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 6

  X = (posX+(largh/4)*3)  - (largh/2);
  Y = (posY+(lungh/2))    - (lungh/2);

  points[5] = Point(X/Rap*Rid, Y/Rap*Rid);

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  points_ruot[5] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  //PUNTO 7

  X = (posX+(largh/4)*3)  - (largh/2);
  Y = posY                - (lungh/2);

  points[6] = Point(X/Rap*Rid, Y/Rap*Rid);
  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-posX;
  Y2 = Y-posY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) - (Y2 * sin);
  Y3 = (X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+posX;
  Y4= Y3+posY;

  points_ruot[6] = Point(X4/Rap*Rid, Y4/Rap*Rid);

  Sfondo->Canvas->Pen->Color   = col;
  //-----------------------------------------------
  // Lampeggio nero.
  //-----------------------------------------------
  Sfondo->Canvas->Brush->Color = clSFONDO;
  //-----------------------------------------------
  // visualizzazione trasparente
  //-----------------------------------------------
  Sfondo->Canvas->Brush->Style = bsClear;

  //-----------------------------------------------
  // Freccia
  //-----------------------------------------------
  //Sfondo->Canvas->Polygon(points, 6);

  //-----------------------------------------------
  // Freccia ruotata
  //-----------------------------------------------
  Sfondo->Canvas->Polygon(points_ruot, 6);

  return;
}

//---------------------------------------------------------------------------

