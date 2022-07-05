//---------------------------------------------------------------------------
#ifndef ImpiantiH
#define ImpiantiH
//---------------------------------------------------------------------------
#include "Tecnofer.h"

//---------------------------------------------------------------------------
// --------------------------------
// Costanti associate all'impianto
// --------------------------------
//
// ...
// #define ....

// --------------------------------
//      Variabili Globali
// --------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif

// ---------------------------------------------------------
//  Funzioni speciali riservate agli Impianti IMPIANTI.CPP
// ---------------------------------------------------------
  void disegna_speciali_IMPIANTI();

  void ReadIni_IMPIANTI();
  void WriteIni_IMPIANTI();

  void VerificaInputMAG_IMPIANTI(struct MAG *mag);

  void VerificaInputBOX_IMPIANTI(struct BOX *box);

  void SetResetPIntForzatoTGV_IMPIANTI(short int num_agv, bool *ResetMissione, bool *ResetChiamata, bool *ResetPIntForzato, short int *ForzaPInt);

  void AvvicinamentoADestinazione_IMPIANTI(short int dest, short int *perc, struct DATI_PERCORSO *dati, short int *pint);

  int  PosizioneRiposo_IMPIANTI(short int num_agv, char *all_mess);

  void AdjustDMStartMission_IMPIANTO(short int num_agv, WORD *dm);

  int load_chiamate_baie_IMPIANTI(char *all_mess);
  int write_chiamate_baie_IMPIANTI(char *all_mess);

#endif;
