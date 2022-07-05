//---------------------------------------------------------------------------
// ------------------------------------------------------------------------
//                              file IngomproAgv.CPP
// ------------------------------------------------------------------------
#include <mem.h>
#include <string.h>
#include <math.h>
#include <share.h>
#include <sys\stat.h>

#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#pragma hdrstop

#include "Variabil.h"
#include "Tecnofer.h"

extern int Percorso[][2];
extern int Punti[][5];
extern int PercorsoCurvaBezier[][7];
extern int IngombroAgvGraph[][3];

extern int   offset[][2];
extern float Rap;

// --------------------------------
//       ingombroagv()
// --------------------------------
// Costruttore classe
//
ingombroagv::ingombroagv(int NumAgv)
/*******************/
{
  this->NumAgv = NumAgv;

  DistA=DIST_AVANTI_INGOMBRO_AGV;
  if(IngombroAgvGraph[NumAgv][0]>0) DistA=IngombroAgvGraph[NumAgv][0];
  DistI=DIST_INDIETRO_INGOMBRO_AGV;
  if(IngombroAgvGraph[NumAgv][1]>0) DistI=IngombroAgvGraph[NumAgv][1];
  DistL=DIST_LATERALE_INGOMBRO_AGV;
  if(IngombroAgvGraph[NumAgv][2]>0) DistL=IngombroAgvGraph[NumAgv][2];

  memset(AreaOccupata, 0, sizeof(AreaOccupata));
}
// --------------------------------
//       AzzeraIngombroAgv()
// --------------------------------
//--------------------------------------------------------------------------------------
// Azzero la struttura AreaOccupata che definisce l'ingombro
//
void ingombroagv::AzzeraIngombroAgv()
{
  memset(AreaOccupata, 0, sizeof(AreaOccupata));
}

// --------------------------------
//       SettaIngombroAgv()
// --------------------------------
//--------------------------------------------------------------------------------------
// In base ai nodi passati setto la struttura AreaOccupata che definisce l'ingombro
//
int ingombroagv::SettaIngombroSuTratto(int NodoStart, int NodoEnd, int DirRot, char *all_mess)
{
  int i, idx_segmento;
  int x_start, x_end, y_start, y_end, x_vert, y_vert;
  int x, y, x1, x2, y1, y2;
  bool il_percorso_e_una_curva=false;
  bool il_percorso_e_un_punto=false;
  double xt,yt;
  double ax,ay;
  double bx,by;
  double t, alpha;
  double step;


  memset(all_mess, 0, sizeof(all_mess));
  strcpy(all_mess, "OK");
  memset(AreaOccupata, 0, sizeof(AreaOccupata));
  //------------------------------------------------
  // Controlli sull'affiadabilita dei dati
  //------------------------------------------------
  if(NodoStart<=0)                 return 1;
  if(NodoStart>TOTPUNTI)           return 1;
  if(NodoEnd<=0)                   return 1;
  if(NodoEnd>TOTPUNTI)             return 1;
  if(N.n[NodoStart]->num<=0)       return 1;
  if(N.n[NodoStart]->num>TOTPUNTI) return 1;
  if(N.n[NodoEnd]->num<=0)         return 1;
  if(N.n[NodoEnd]->num>TOTPUNTI)   return 1;

  //----------------------------------------------------------
  // Controlla se ho un solo punto
  //----------------------------------------------------------
  if(NodoStart==NodoEnd){
     il_percorso_e_un_punto=true;
     x_start = Punti[NodoStart][0];
     y_start = Punti[NodoStart][1];
     x_end   = Punti[NodoEnd][0];
     y_end   = Punti[NodoEnd][1];
  }
  //----------------------------------------------------------
  // Controlla se i nodi sono gli estremi di una curva
  //----------------------------------------------------------
  if(!il_percorso_e_un_punto){
     for(i=0;;i++){
        if(PercorsoCurvaBezier[i][0]==0) break;
        if((PercorsoCurvaBezier[i][1]==NodoStart && PercorsoCurvaBezier[i][2]==NodoEnd)  ||
           (PercorsoCurvaBezier[i][1]==NodoEnd   && PercorsoCurvaBezier[i][2]==NodoStart)   ){
           il_percorso_e_una_curva=true;
           //-----------------------------------------------------------------
           // ricavo le coordinete dei punti
           //-----------------------------------------------------------------
           if(PercorsoCurvaBezier[i][1]==NodoStart && PercorsoCurvaBezier[i][2]==NodoEnd){
              x_start = Punti[PercorsoCurvaBezier[i][1]][0];
              y_start = Punti[PercorsoCurvaBezier[i][1]][1];
              x_end   = Punti[PercorsoCurvaBezier[i][2]][0];
              y_end   = Punti[PercorsoCurvaBezier[i][2]][1];
           }
           else{
              x_start = Punti[PercorsoCurvaBezier[i][2]][0];
              y_start = Punti[PercorsoCurvaBezier[i][2]][1];
              x_end   = Punti[PercorsoCurvaBezier[i][1]][0];
              y_end   = Punti[PercorsoCurvaBezier[i][1]][1];
           }
           x_vert = PercorsoCurvaBezier[i][3];
           y_vert = PercorsoCurvaBezier[i][4];
        }
     }
  }
  //------------------------------------------------------------------------------------
  // Se il percorso non è una curva e non è un punto lo considero un tratto rettilineo
  //------------------------------------------------------------------------------------
  if(!il_percorso_e_una_curva && !il_percorso_e_un_punto){
     x_start = Punti[NodoStart][0];
     y_start = Punti[NodoStart][1];
     x_end   = Punti[NodoEnd][0];
     y_end   = Punti[NodoEnd][1];
  }
  //-----------------------------------------------------------------------
  // Occupazione lungo una CURVA
  //-----------------------------------------------------------------------
  if(il_percorso_e_una_curva){

     // calcola i coefficienti

     bx = 2 * (x_vert - x_start);
     ax = x_end - x_start - bx;

     by = 2 * (y_vert - y_start);
     ay = y_end - y_start - by;

     idx_segmento=0;
     t = 0;
     step = 0.05;

     while (t <= 1.04) {  // STRANO !!! Se metto <=1 l'ultimo step con t=1 non me lo esegue!!!
        xt = x_start + t * (x_vert - x_start) ;
        yt = y_start + t * (y_vert - y_start) ;

        /*
        x1 = (int) floor(xt + 0.5);
        y1 = (int) floor(yt + 0.5);
        */
        x1 = (int) xt;
        y1 = (int) yt;

        xt = x_vert + t * (x_end - x_vert) ;
        yt = y_vert + t * (y_end - y_vert) ;

        /*
        x2 = (int) floor(xt + 0.5);
        y2 = (int) floor(yt + 0.5);
        */
        x2 = (int) xt;
        y2 = (int) yt;

        if((x2!=x1)){
           alpha =(double) atan((double)((double)(y1-y2)/(double)(x2-x1)));
           if(x2<x1){
              alpha=alpha+M_PI;
           }
        }
        else{
           if(y1>=y2) alpha = M_PI_2;
           else               alpha = M_PI_2*3;
        }
        alpha = alpha - M_PI_2; // la nostra "gestione gradi" è ruotata di 90 gradi riapetto alla circonferenza goniometrica classica

        if(DirRot==1) alpha=alpha+M_PI;

        xt = ax * pow(t, 2) + bx * t + x_start;
        yt = ay * pow(t, 2) + by * t + y_start;
        /*
        x = (int) floor(xt + 0.5);
        y = (int) floor(yt + 0.5);
        */
        x = (int) xt;
        y = (int) yt;

        calcola_ingombro_agv_su_singolo_punto(x, y, alpha, &AreaOccupata[idx_segmento]);
        idx_segmento=idx_segmento+4;

        t += step;
     }
  }
  //-----------------------------------------------------------------------
  // Occupazione in un PUNTO
  //-----------------------------------------------------------------------
  if(il_percorso_e_un_punto){
     alpha=DegToRad(DirRot);
     calcola_ingombro_agv_su_singolo_punto(x_start, y_start, alpha, AreaOccupata);
  }
  //-----------------------------------------------------------------------
  // Occupazione lungo una RETTA
  //-----------------------------------------------------------------------
  if(!il_percorso_e_una_curva && !il_percorso_e_un_punto){
     if((x_end!=x_start)){
        //alpha =(double) atan((double)((double)((y_end*(-1))-(y_start*(-1)))/(double)(x_end-x_start)));
        alpha =(double) atan((double)((double)(y_start-y_end)/(double)(x_end-x_start)));
        if(x_end<x_start){
           alpha=alpha+M_PI;
        }
     }
     else{
        if(y_start>=y_end) alpha = M_PI_2;
        else               alpha = M_PI_2*3;
     }
     alpha = alpha - M_PI_2; // la nostra "gestione gradi" è ruotata di 90 gradi riapetto alla circonferenza goniometrica classica
     if(DirRot==1) alpha=alpha+M_PI;

     idx_segmento=0;
     t = 0;
     step = 0.1;
     while (t < 1) {
        xt = x_start + t * (x_end - x_start);
        yt = y_start + t * (y_end - y_start);
        /*
        x = (int) floor(xt + 0.5);
        y = (int) floor(yt + 0.5);
        */
        x = (int) xt;
        y = (int) yt;

        calcola_ingombro_agv_su_singolo_punto(x, y, alpha, &AreaOccupata[idx_segmento]);
        idx_segmento=idx_segmento+4;

        t += step;
     }
  }

  return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ----------------------------------------------
//       calcola_ingombro_agv_su_singolo_punto()
// ----------------------------------------------
void ingombroagv::calcola_ingombro_agv_su_singolo_punto(int PosX, int PosY, double rot, struct LINE *SegmentiIngombroAgv)
{

  long X, Y, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
  long double sin, cos;
  TPoint points_ruot[4];

  SinCos(rot, sin, cos);

  //PUNTO 1
  //-----------------------------------------------
  // Disegno poligono postazione singola
  //-----------------------------------------------
//X = PosX  - DIST_LATERALE_INGOMBRO_AGV;
//Y = PosY  - DIST_AVANTI_INGOMBRO_AGV;
  X = PosX  - DistL;
  Y = PosY  - DistA;

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-PosX;
  Y2 = Y-PosY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) + (Y2 * sin);
  Y3 = -(X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+PosX;
  Y4 = Y3+PosY;

  points_ruot[0] = Point(X4, Y4);

  //PUNTO 2
//X = PosX - DIST_LATERALE_INGOMBRO_AGV;
//Y = PosY + DIST_INDIETRO_INGOMBRO_AGV;
  X = PosX - DistL;
  Y = PosY + DistI;

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-PosX;
  Y2 = Y-PosY;

  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) + (Y2 * sin);
  Y3 = -(X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+PosX;
  Y4 = Y3+PosY;

  points_ruot[1] = Point(X4, Y4);

  //---------------------------------------------------------------------
  // Memorizzo il primo segmento nei segmenti di ingombro della navetta
  //---------------------------------------------------------------------
  SegmentiIngombroAgv[0].p1.x = points_ruot[0].x;
  SegmentiIngombroAgv[0].p1.y = points_ruot[0].y;
  SegmentiIngombroAgv[0].p2.x = points_ruot[1].x;
  SegmentiIngombroAgv[0].p2.y = points_ruot[1].y;

  //PUNTO 3
//X = PosX + DIST_LATERALE_INGOMBRO_AGV;
//Y = PosY + DIST_INDIETRO_INGOMBRO_AGV;
  X = PosX + DistL;
  Y = PosY + DistI;

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-PosX;
  Y2 = Y-PosY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) + (Y2 * sin);
  Y3 = -(X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+PosX;
  Y4 = Y3+PosY;

  points_ruot[2] = Point(X4, Y4);
  //---------------------------------------------------------------------
  // Memorizzo il secondo segmento nei segmenti di ingombro della navetta
  //---------------------------------------------------------------------
  SegmentiIngombroAgv[1].p1.x = points_ruot[1].x;
  SegmentiIngombroAgv[1].p1.y = points_ruot[1].y;
  SegmentiIngombroAgv[1].p2.x = points_ruot[2].x;
  SegmentiIngombroAgv[1].p2.y = points_ruot[2].y;

  //PUNTO 4
//X = PosX + DIST_LATERALE_INGOMBRO_AGV;
//Y = PosY - DIST_AVANTI_INGOMBRO_AGV;
  X = PosX + DistL;
  Y = PosY - DistA;

  //----------------------------------------
  // Coordinate rispetto alla nuova origine
  //----------------------------------------
  X2 = X-PosX;
  Y2 = Y-PosY;
  //--------------------------------------
  // Ruoto rispetto alla nuova origine
  //--------------------------------------
  X3 = (X2 * cos) + (Y2 * sin);
  Y3 = -(X2 * sin) + (Y2 * cos);
  //----------------------------------------------
  // Riporto le coordinate  alla vecchia origine
  //----------------------------------------------
  X4 = X3+PosX;
  Y4 = Y3+PosY;

  points_ruot[3] = Point(X4, Y4);
  //---------------------------------------------------------------------
  // Memorizzo il terzo segmento nei segmenti di ingombro della navetta
  //---------------------------------------------------------------------
  SegmentiIngombroAgv[2].p1.x = points_ruot[2].x;
  SegmentiIngombroAgv[2].p1.y = points_ruot[2].y;
  SegmentiIngombroAgv[2].p2.x = points_ruot[3].x;
  SegmentiIngombroAgv[2].p2.y = points_ruot[3].y;
  //---------------------------------------------------------------------
  // Memorizzo il quarto segmento nei segmenti di ingombro della navetta
  //---------------------------------------------------------------------
  SegmentiIngombroAgv[3].p1.x = points_ruot[3].x;
  SegmentiIngombroAgv[3].p1.y = points_ruot[3].y;
  SegmentiIngombroAgv[3].p2.x = points_ruot[0].x;
  SegmentiIngombroAgv[3].p2.y = points_ruot[0].y;

  return;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ----------------------------------------------
//       disegna_ingombro_agv()
// ----------------------------------------------
void ingombroagv::disegna_ingombro_agv(ingombroagv **ListaAgv, int TotAgv, Graphics::TBitmap *Sfondo)
{
  int i=0, j=0, k=0;
  TPoint points[4];

  for(;i<1000;){
     if(AreaOccupata[i].p1.x==0 && AreaOccupata[i].p1.y==0)   break;
     points[0].x= ((AreaOccupata[i].p1.x-offset[1][0])/Rap)*Rid;
     points[0].y= ((AreaOccupata[i].p1.y-offset[1][1])/Rap)*Rid;
     points[1].x= ((AreaOccupata[i].p2.x-offset[1][0])/Rap)*Rid;
     points[1].y= ((AreaOccupata[i].p2.y-offset[1][1])/Rap)*Rid;

     points[2].x= ((AreaOccupata[i+1].p2.x-offset[1][0])/Rap)*Rid;
     points[2].y= ((AreaOccupata[i+1].p2.y-offset[1][1])/Rap)*Rid;

     points[3].x= ((AreaOccupata[i+2].p2.x-offset[1][0])/Rap)*Rid;
     points[3].y= ((AreaOccupata[i+2].p2.y-offset[1][1])/Rap)*Rid;

     /*
     pointsNav[0].x= (((AreaOccupata[i].p1.x+10)-offset[1][0])/Rap)*Rid;
     pointsNav[0].y= (((AreaOccupata[i].p1.y+10)-offset[1][1])/Rap)*Rid;
     pointsNav[1].x= (((AreaOccupata[i].p2.x-10)-offset[1][0])/Rap)*Rid;
     pointsNav[1].y= (((AreaOccupata[i].p2.y-10)-offset[1][1])/Rap)*Rid;

     pointsNav[2].x= (((AreaOccupata[i+1].p2.x-10)-offset[1][0])/Rap)*Rid;
     pointsNav[2].y= (((AreaOccupata[i+1].p2.y-10)-offset[1][1])/Rap)*Rid;

     pointsNav[3].x= (((AreaOccupata[i+2].p2.x-10)-offset[1][0])/Rap)*Rid;
     pointsNav[3].y= (((AreaOccupata[i+2].p2.y-10)-offset[1][1])/Rap)*Rid;
     */
     i=i+4;

     Sfondo->Canvas->Pen->Color = clGreen;

     for(j=0;j<TotAgv;j++){
        if(ListaAgv[j]->NumAgv==this->NumAgv) continue;
        //---------------------------------------------------------------------------------
        // Controllo se uno qualsiasi dei segmenti interseca un segmento dell'altra curva
        //---------------------------------------------------------------------------------
        for(k=0;k<1000;k++){
           if(ListaAgv[j]->AreaOccupata[k].p1.x==0 && ListaAgv[j]->AreaOccupata[k].p1.y==0)  break;
           if(intersect(AreaOccupata[i-4], ListaAgv[j]->AreaOccupata[k])){
              Sfondo->Canvas->Pen->Color = clRed;
              break;
           }
           if(intersect(AreaOccupata[i-3], ListaAgv[j]->AreaOccupata[k])){
              Sfondo->Canvas->Pen->Color = clRed;
              break;
           }
           if(intersect(AreaOccupata[i-2], ListaAgv[j]->AreaOccupata[k])){
              Sfondo->Canvas->Pen->Color = clRed;
              break;
           }
           if(intersect(AreaOccupata[i-1], ListaAgv[j]->AreaOccupata[k])){
              Sfondo->Canvas->Pen->Color = clRed;
              break;
           }
        }
      }



     //-----------------------------------------------
     // Disegno rettangolo
     //-----------------------------------------------
     Sfondo->Canvas->Brush->Color = clBlack;
     Sfondo->Canvas->Brush->Style = bsClear;

     Sfondo->Canvas->Polygon(points, 3);

     Sfondo->Canvas->Pen->Color = clWhite;


     Sfondo->Canvas->MoveTo(points[3].x, points[3].y);
     Sfondo->Canvas->LineTo(points[0].x, points[0].y);
     

  }
  return;
  //-----------------------------------------------

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ----------------------------------------------
//       VerificaIntersezioneAgv()
// ----------------------------------------------
int ingombroagv::VerificaIntersezioneAgv(ingombroagv *AltroAgv, char *all_mess)
{
  int i, k;
  //---------------------------------------------------------------------------------
  // Controllo se uno qualsiasi dei segmenti interseca un segmento dell'altra curva
  //---------------------------------------------------------------------------------
  for(i=0;i<1000;i++){
    if(AreaOccupata[i].p1.x==0 && AreaOccupata[i].p1.y==0)  break;
    for(k=0;k<1000;k++){
       if(AltroAgv->AreaOccupata[k].p1.x==0 && AltroAgv->AreaOccupata[k].p1.y==0)  break;
       if(intersect(AreaOccupata[i], AltroAgv->AreaOccupata[k])){
          return 1;
       }
    }
  }

  return 0;
}
//---------------------------------------------------------------------------
int ingombroagv::ccw (TPoint p0, TPoint p1, TPoint p2)
{
   int dx1,dx2,dy1,dy2;

   dx1 = p1.x-p0.x;
   dy1 = p1.y-p0.y;
   dx2 = p2.x-p0.x;
   dy2 = p2.y-p0.y;

   if (dx1*dy2 > dy1*dx2) return +1;
   if (dx1*dy2 < dy1*dx2) return -1;
   if ((dx1*dx2 < 0) || (dy1*dy2 < 0)) return -1;
   if ((dx1*dx1+dy1*dy1) < (dx2*dx2+dy2*dy2)) return +1;
   return 0;
}
int ingombroagv::intersect(struct LINE l1, struct LINE l2)
{
   return ((ccw(l1.p1,l1.p2,l2.p1)*ccw(l1.p1,l1.p2,l2.p2)) <=0) &&
          ((ccw(l2.p1,l2.p2,l1.p1)*ccw(l2.p1,l2.p2,l1.p2)) <=0);
}
#pragma package(smart_init)
