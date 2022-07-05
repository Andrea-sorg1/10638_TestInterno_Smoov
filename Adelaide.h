//---------------------------------------------------------------------------
#ifndef AdelaideH
#define AdelaideH
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


//-----------------------------------------------------------
// Variabili globali riservate agli impianti OI_ADELAIDE
//-----------------------------------------------------------
#define GRP_BAIE_ADELAIDE        1
#define GRP_MAG_TEMP_ADELAIDE    2
#define GRP_MAG_CARWASH_ADELAIDE 3
#define GRP_BAIA_RORO            4
#define GRP_MAG_SPED10_ADELAIDE  5
#define GRP_MAG_SPED11_ADELAIDE  6
#define GRP_MAG_SPED12_ADELAIDE  7
#define GRP_MAG_SPED13_ADELAIDE  8
#define GRP_MAG_SPED14_ADELAIDE  9
#define GRP_MAG_SPED15_ADELAIDE 10
#define GRP_MAG_SPED16_ADELAIDE 11
#define GRP_MAG_SPED17_ADELAIDE 12
#define GRP_MAG_SPED18_ADELAIDE 13
#define GRP_MAG_EMERGENZA_RORO  14

TIPO_VARIABILE WORD WatchDog;
TIPO_VARIABILE bool AttesaBaieGiaFatta[TOTBAIE+1];
TIPO_VARIABILE short int TimeAttesaBaiePieni_ADELAIDE;
TIPO_VARIABILE short int TimeAttesaBaiaRORO_ADELAIDE;
TIPO_VARIABILE short int AbilitaAttesaLaserUscitaMag_ADELAIDE;
TIPO_VARIABILE short int AbilitaLetturaDatiDaPlc_ADELAIDE;
TIPO_VARIABILE bool AbilitaLetturaBassoDaPlc_ADELAIDE;
TIPO_VARIABILE bool AbilitaCarWashPerMagSaturo_ADELAIDE;
TIPO_VARIABILE bool DESTSQLdaTecno_ADELAIDE;
TIPO_VARIABILE bool NoAttesaDaRicarica_ADELAIDE;
TIPO_VARIABILE bool AbilitaDMAltezzaSuScaricoAMag_ADELAIDE;


// ---------------------------------------------------------
//  Funzioni speciali riservate agli Impianti IMPIANTI.CPP
// ---------------------------------------------------------
  void ReadIni_ALLGLASS();
  void WriteIni_ALLGLASS();

  void VerificaInputMAG_ALLGLASS(struct MAG *mag);

  void VerificaInputBOX_ALLGLASS(struct BOX *box);

  int  PosizioneRiposo_ALLGLASS(short int num_agv, char *all_mess);

  void SetResetPIntForzatoTGV_ALLGLASS(short int num_agv, bool *ResetMissione, bool *ResetChiamata, bool *ResetPIntForzato, short int *ForzaPInt);

  void AvvicinamentoADestinazione_ALLGLASS(short int dest, short int *perc, struct DATI_PERCORSO *dati, short int *pint);

  void AdjustDMStartMission_ALLGLASS(short int num_agv, WORD *dm);

  int load_chiamate_baie_ALLGLASS(char *all_mess);
  int write_chiamate_baie_ALLGLASS(char *all_mess);


  int  load_udc_from_SQL_ALLGLASS(int cmd, struct DATI_PRD *UdcTmp, unsigned int locS, unsigned int locE, char *UdcS, char *UdcE, char *all_mess);
  int  write_udc_to_SQL_ALLGLASS( int cmd, struct DATI_PRD *UdcTmp, char *all_mess);


#endif;
