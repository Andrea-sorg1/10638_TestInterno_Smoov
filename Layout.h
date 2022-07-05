//---------------------------------------------------------------------------
#ifndef LayoutH
#define LayoutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include "Variabil.h"
#include "CGAUGES.h"
#include <Graphics.hpp>
// riattiva SfondoGrafico
//#include "SfondoGrafico_TLB.h"
//---------------------------------------------------------------------------
#define max(a, b)  (((a) > (b)) ? (a) : (b))   // ING.
#define min(a, b)  (((a) < (b)) ? (a) : (b))   // ING.
//---------------------------------------------------------------------------
class TFormLayout : public TForm
{
__published:	// IDE-managed Components
  TStaticText *TextDati;
  TEdit *EditGrd;
  TUpDown *UpDown1;
  TLabel *LabelMain;
  TEdit *EditOff;
  TUpDown *UpDwnOff;
  TPanel *GaugePanel;
  TCGauge *CGauge1;
  TImage *RaceTGV;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled);
  void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled);

private:	// User declarations

  //-------------------------------------------------------
  // Colori BASE
  //-------------------------------------------------------
  TColor clNUMLOC;
  TColor clSFONDO;
  TColor clLINEE;
  TColor clNODI;
  TColor clNODI_FING;
  //-------------------------------------------------------
  // Visualizzazione dati carrelli
  //-------------------------------------------------------
  void __fastcall OnHint(TObject *Sender);
  //-------------------------------------------------------
  // Variabili di gestione Form principale
  //-------------------------------------------------------
  Graphics::TBitmap *Sfondo;
  Graphics::TBitmap *Sfondo1;
  //-------------------------------------------------------
  // Sfondo Grafico
  //-------------------------------------------------------
  struct BLOCCOSEMPLICE{
     int X;
     int Y;
     int LatoX;
     int LatoY;
     int Tipo;
     int ColS;
     int ColB;
  };
  struct BLOCCOSPECIALE{
     struct BLOCCOSEMPLICE Blocco[MAXBLOCCHISEMPLICI+1];
  };

  //riattiva SfondoGrafico (da commentare)

  struct BLOCCOGRAFICO{
     int X;
     int Y;
     int LatoX;
     int LatoY;
     short Rotazione;
     int ColPen;
     int ColBrush;
     signed char Stringa[51];
     int ColStringa;
     short Tipo;
     signed char Selezionato;
     short Sviluppo;
     short Speciale;
  };

  //riattiva SfondoGrafico (da commentare)

  //-------------------------------------------------------
  // Gestione test manuale TecnoFerrari
  //-------------------------------------------------------
  short int EndTestTecno;
  short int StartTestTecno;
  short int TimeTestTecno;

  //-------------------------------------------------------
  // Variabili grafiche di sistema
  //-------------------------------------------------------
  bool  AbilitaZoom;
  bool  SpostManAttivo;
  int   SpostManX;
  int   SpostManY;
  int   ZoomX1,ZoomX2,ZoomY1,ZoomY2,ZoomSel,ZoomOk;

  //-------------------------------------------------
  // Funzione di Zoom Sinottico
  //-------------------------------------------------
  void  Zoom_Dinamico_Mouse();
  //-------------------------------------------------------
  // Funzioni di inizializzazione grafica del sistema
  //-------------------------------------------------------
  void  InizializzaVariabiliGrafiche();
  //-------------------------------------------------------
  // Funzioni di disegna del layout grafico
  //-------------------------------------------------------
  void  StampNum( Graphics::TBitmap *Bmp, int x, int y, char *num, TColor col, bool Little, int Down);
  void  StampaStringa( Graphics::TBitmap *Bmp, int x, int y, char *stringa, TColor col);
  void  Spezza_Codice(char *codice, char *cod1, char *cod2, int parte);
  void  SfondoGrafico();
  void  BloccoGrafico(int X, int Y, int LatoX, int LatoY, TColor ColS, TColor ColB, TColor ColM, char *Testo, int Tipo, int Rot, bool Sel);
  void  linea_punti( int source, int dest, TColor col );  // linea
  void  curva_punti( int source, int dest, int tipo, int raggio, TColor col );  // curva
  void  curva_punti_bezier( TPoint *p, short int tot_p, TColor col);  // bezier
  void  modifica_curva_punti_bezier( TPoint *p, short int tot_p);

  void  CurvaGenerica( int X1, int Y1, int X2, int Y2, int tipo, int raggio, TColor col, int Riempimento );  // curva
  int   compila_nodi_graf();                      // per la colorazione dei percorsi
                                                  // con il software in AUTOMATICO.
  void  disegna_percorso();                       // disegna percorso
  void  disegna_percorso_tgv();                   // disegna percorso dinamico
  void  disegna_test_percorsi();                  // disegna percorso TEST TECNOFERRARI
  void  disegna_macchine();                       // disegna MACCHINE
  void  disegna_baie();                         // disegna BAIE
  void  disegna_magazzini();                      // disegna MAGAZZINI
  void  disegna_porte();                          // disegna PORTE
  void  disegna_cbatt();                          // disegna CAMBIO BATTERIA
  void  disegna_agv( int Num );                   // disegna TGV (FORCHE)
  void  disegna_agv_box( int Num );               // disegna TGV ( BOX  )
  void  disegna_numeri();                         // disegna numeri nodi, mag, mac
  void  disegna_numero_richiesto();               // disegna numero richiesto da POSIZIONAMENTO SU NODO
  void  DisegnaRiflettoriLaser( int Num );        // disegna Riflettori (GESTIONE LASER)
  void  DisegnaBox(short int PresCEST_A, short int PresCEST_B, TColor *ColBOX_A, TColor *ColBOX_B);

public:		// User declarations


  bool VisualizzaTgvTrasparente[MAXAGV+1];        // visualizza TGV sempre trasparente
  bool VisualizzaZoneImpianto;                    // visualizza zone impianto
  bool VisualizzaZoneMapping;
  bool AbilitaSfondoGrafico;                      // visualizza sfondo grafico
  bool TestPercorsiTecnoFerrari;                  // test percorsi TecnoFerrari

  void  AggiornaSinottico();                      // aggiornamento sinottico
//riattiva SfondoGrafico
//TCOMITSfondoGrafico COMISfondo;                 // ActiveX di gestione SfondoGrafico

  struct BLOCCOGRAFICO  BloccoG[MAXBLOCCHIGRAFICI+1];
  struct BLOCCOSPECIALE BloccoS[MAXBLOCCHISPECIALI+1];

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RRRRRRRRRRRRRRRRRRRRRRR
  struct POS_AGV{
       long double xc,yc;
       int x1,x2;
       int y1,y2;
  };
  POS_AGV Pos_Agv[MAXAGV+1];
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RRRRRRRRRRRRRRRRRRRRRRR

  short int PosizionamentoSuNodo;

  void  disegna_freccia(int PosX, int PosY, int largh, int Lungh, int gradi);
  void  disegna_freccia2(int PosX, int PosY, int largh, int lungh, int gradi, TColor col);

  //-----------------------------------
  // PRECEDENZE PER TGV IN RETTILINEO
  //-----------------------------------
  bool VisualizzaTgvLanciatiInRettilineo;
  //----------------------------------------
  // Variabili di gestione layout
  //----------------------------------------
  int   FormH, FormW, SfondoH, SfondoW;

  __fastcall TFormLayout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLayout *FormLayout;
//---------------------------------------------------------------------------
#endif
