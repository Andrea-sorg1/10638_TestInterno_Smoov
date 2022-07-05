//---------------------------------------------------------------------------
#ifndef IcamH
#define IcamH
//---------------------------------------------------------------------------
#include "Tecnofer.h"
#include "Layout.h"

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
#endif;



TIPO_VARIABILE WORD WatchDog;


// ---------------------------------------------------------
//  Funzioni speciali riservate agli Impianti IMPIANTI.CPP
// ---------------------------------------------------------
  void disegna_speciali_ICAM();

  void ReadIni_ICAM();
  void WriteIni_ICAM();

  void VerificaInputMAG_ICAM(struct MAG *mag);

  void VerificaInputBOX_ICAM(struct BOX *box);

  int  PosizioneRiposo_ICAM(short int num_agv, char *all_mess);

  void SetResetPIntForzatoTGV_ICAM(short int num_agv, bool *ResetMissione, bool *ResetChiamata, bool *ResetPIntForzato, short int *ForzaPInt);

  void AvvicinamentoADestinazione_ICAM(short int dest, short int *perc, struct DATI_PERCORSO *dati, short int *pint);

  void AdjustDMStartMission_ICAM(short int num_agv, WORD *dm);

  int load_chiamate_baie_ICAM(char *all_mess);
  int write_chiamate_baie_ICAM(char *all_mess);


  int  load_udc_from_SQL_ICAM(int cmd, struct DATI_PRD *UdcTmp, unsigned int locS, unsigned int locE, char *UdcS, char *UdcE, char *all_mess);
  int  write_udc_to_SQL_ICAM(int cmd, struct DATI_PRD *UdcTmp, char *all_mess);


#endif;
