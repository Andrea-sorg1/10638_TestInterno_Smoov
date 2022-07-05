//---------------------------------------------------------------------------
#ifndef StampaH
#define StampaH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include "perfgrap.h"
#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <TeeFunci.hpp>
//---------------------------------------------------------------------------
#define  MARGINE_STAMPA_SOTTO   400   // parametro per la stampa di quanto spazio bianco lasciare in fondo alla tabella
                                      // (lo spazio bianco serve a metterci n eventuale totale)
//---------------------------------------------------------------------------
class Stampa {
     public:

  TPrinter *Prntr;
  int   Xd[21];
  bool  pag;

  bool  Orientation;
  int   Colonne;
  int   SizeIntestazione;
  int   SizeIntColonne;
  int   LarghezzeCol[20];
  char  TipoFont[21];
  char  Intestazione[101];
  char  Totali[101];
  char  IntCol[21][51];
  char  Dati[21][101];

  Stampa();

  void  StampaIntestazione();
  void  StampaCampi();
  void  StampaTotali();
  void  FineStampa();

  // la funzione StampaGenerica(..) si riesce ad utilizzare
  // solo se i tipi di dati da stampare provengono da un file che crea una lista.
  void  StampaGenerica(int Tipo, char fileDAT[15]);                             // Con il Tipo si identifica la stampa che si vuol fare
                                                                                // 0: Storico missioni.
                                                                                // 1: Cronologico macchine.
                                                                                // ...
};
//---------------------------------------------------------------------------

#endif


