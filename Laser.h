//---------------------------------------------------------------------------
#ifndef LaserH
#define LaserH

#include "tecnofer.h"

//---------------------------------------------------------------------------

// --------------------------------
//      Variabili Globali
// --------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif

#define LayerSection   "LAYER"
#define LayerKeyXL     "XLayer"
#define LayerKeyYL     "YLayer"
#define LayerKeyAL     "ALayer"
#define LayerKeyRL     "RLayer"
#define LayerKeyMinR   "MinRifl"
#define LayerKeyNR     "NRifl"
#define LayerKeyRR     "RRifl"


#define RifletSection  "RIFLET"
#define RifletKeyXR    "XRifl"
#define RifletKeyYR    "YRifl"

#define LayerKeyNodo    "NODO_ZONA"
#define LayerKeyMaxZone "MAX_ZONE"

// --------------------------------
//      Strutture Globali
// --------------------------------

TIPO_VARIABILE int LaserMappaRiflettoriTgv;   // TGV  di cui si vogliono vedere i riflettori
TIPO_VARIABILE int LaserMappaRiflettoriZona;  // Zona di cui si vogliono vedere i riflettori (99=tutte)

TIPO_VARIABILE int LaserComandoInCorso;
TIPO_VARIABILE int LaserInterrompiComando;

TIPO_VARIABILE int  MsecTimeOutSA;
TIPO_VARIABILE int  MsecTimeOutPA;
TIPO_VARIABILE int  MsecTimeOutPC;
TIPO_VARIABILE int  MsecTimeOutPP;
TIPO_VARIABILE int  MsecTimeOutPL;
TIPO_VARIABILE int  MsecTimeOutMA;
TIPO_VARIABILE int  MsecTimeOutMS;
TIPO_VARIABILE int  MsecTimeOutMM;
TIPO_VARIABILE int  MsecTimeOutMN;
TIPO_VARIABILE int  MsecTimeOutMR;
TIPO_VARIABILE int  MsecTimeOutDA;
TIPO_VARIABILE int  MsecTimeOutDR;
TIPO_VARIABILE int  MsecTimeOutUA;
TIPO_VARIABILE int  MsecTimeOutUR;
TIPO_VARIABILE int  MsecTimeOutRS;
TIPO_VARIABILE int  G_MinimoMediaMap;
TIPO_VARIABILE int  Laser_RotationMode;           // Rotazione tasta Laser 0=Antioraria 1=Oraria
TIPO_VARIABILE bool Laser_AbilitaZonaMapSuNodo;   // Abilita possibilità di impostare (forzare) Zona mapping da editor nodi 



void LaserProcessaWindowsMessage( int MsecAttesa );
int  LaserWaitConclusioneComandoEthernet( int MsecAttesa );



void ReadIniParametriLaser();
void WriteIniParametriLaser();

int LaserConvertiWordToStringa( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int LaserConvertiStringaToWord( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int LaserConvertiIntToStringa( int* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int LaserConvertiStringaToInt( int* ArrayWORD, char* ArrayCHAR, int LenCHAR);

int LaserConvertiDmToStringa( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);
int LaserConvertiStringaToDm( WORD* ArrayWORD, char* ArrayCHAR, int LenCHAR);

int LaserCalcolaBCC(char *stringa, int LenStringa, unsigned char *Bcc, char *all_mess);

int LaserLeggiAreaDmLaser        ( int NumeroTgv, short int* DM,       char* all_mess);
int LaserAbilitaTipoComunicazione( int NumeroTgv, int Modo,            char* all_mess);
int LaserInvioValoreDiTimeOut    ( int NumeroTgv, int MsecTimeOutTrs,  char* all_mess);
int LaserLeggiNumeroSpecchi      ( int NumeroTgv, int* Specchi,        char* all_mess);
int LaserInvioNumeroSpecchi      ( int NumeroTgv, int  Specchi,        char* all_mess);
int LaserInvioNumeroMaxZone      ( int NumeroTgv, int MaxZone,         char* all_mess);
int LaserInvioUltimaZonaMapping  ( int NumeroTgv, int Zona,            char* all_mess);
int LaserProceduraCalibratura    ( int NumeroTgv, int Modo,            char* all_mess);
int LaserInvioRapportoEncoder    ( int NumeroTgv, int Val,             char* all_mess);
int LaserLeggiCentroSterzo       ( int NumeroTgv, int* Val,            char* all_mess);
int LaserInvioCentroSterzo       ( int NumeroTgv, int Val,             char* all_mess);
int LaserLeggiScostamentoPuntoLogico( int NumeroTgv, int* Val,         char* all_mess);


int LaserTrsComandoGenerico( int NumeroTgv, int MsecTimeOutTrs, char *StringaTrasmissione, char *StringaRicezione,
	                           int LenTrasmissione, int LenRicezione, char* all_mess);

int LaserTrsComando_SA( int NumeroTgv, char* all_mess);
int LaserTrsComando_SU( int NumeroTgv, int Rotation, char* all_mess);

int LaserTrsComando_PA( int NumeroTgv, char* all_mess);
int LaserTrsComando_PC( int NumeroTgv, int N_Min, char* all_mess);
int LaserTrsComando_PP( int NumeroTgv, int* X, int* Y, int* A, int* G, int* N_Trovato, char* all_mess);
int LaserTrsComando_PP_Media( int NumeroTgv, int* X, int* Y, int* A, int* G, int* N_Trovato, char* all_mess);
int LaserTrsComando_PL( int NumeroTgv, int E, char* all_mess);


int LaserTrsComando_MA( int NumeroTgv, char* all_mess);
int LaserTrsComando_MS( int NumeroTgv, int E, int X, int Y, int A, int R, int* N_Trovato, char* all_mess);
int LaserTrsComando_MM( int NumeroTgv, int E, int S, int X, int Y, int A, int R, int* N_Trovato, char* all_mess);
int LaserTrsComando_MN( int NumeroTgv, int E, int S, int X, int Y, int A, int R, int* N_Trovato, char* all_mess);
int LaserTrsComando_MR( int NumeroTgv, int E, int N_Richesto, int* N_Trovato, int* X, int* Y,  char* all_mess);

int LaserTrsComando_DA( int NumeroTgv, char* all_mess);
int LaserTrsComando_DR( int NumeroTgv, int E, int N_Richesto, int X, int Y, int* N_Trovato, char* all_mess);

int LaserTrsComando_UA( int NumeroTgv, char* all_mess);
int LaserTrsComando_UR( int NumeroTgv, int E, int* N_Trovato, int* X, int* Y,  char* all_mess);

int LaserTrsComando_RS( int NumeroTgv, int E, int R, char* all_mess);

//int  ReadIniLaser(int NumeroTgv, int E, struct LAYER *Dati, char* all_mess);
//void WriteIniLaser(int NumeroTgv, int E, struct LAYER *Dati);
//int ReadIniGeneraleLaser (  char* all_mess );
//int WriteIniGeneraleLaser(  char* all_mess );

int  LaserWriteTgvMappingTxt(int NumeroTgv, char* FileName, char* all_mess);


//----------------------------------
// Nomenclatura
//----------------------------------
//
// E = Layer o zona (0...xx)
// N = Riflettore
// X = Coordinata X (int)
// Y = Coordinata Y (int)
// A = Angolo in bdeg (WORD 16bit) 0..65536 dec 0..C000 hex  (90°-->16384 bdeg)
//
//

#endif

