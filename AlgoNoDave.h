//---------------------------------------------------------------------------
#ifndef AlgoNoDaveH
#define AlgoNoDaveH
//---------------------------------------------------------------------------
#include <windows.h>

#define ALGO_NODAVE_MAX_PLC  100

#define BCCWIN


#include "NoDave.h"


//#pragma warn -pck

struct AlgoNoDave_Interfaccia{
  //-------------------------------------
  // Relative a un dispositivo seriale
  //-------------------------------------
	HANDLE hComm;
  char ComName[10];
  char Baud[10];

  //---------------------------------------
  // Relative a una comunicazione Ethernet
  //---------------------------------------
  char TcpAddress[20];
  int  TcpPort;

  int TipoCom;    // 0-Seriale MPI, 1 Ethernet TCP
  int localMPI;
  int plcMPI;
  int useSlot;
  int useRec;
  int useProtocol;

  daveInterface * di;
  daveConnection * dc;
  _daveOSserialType fds;
};

extern AlgoNoDave_Interfaccia AlgoDevice[ ALGO_NODAVE_MAX_PLC+1 ];


HANDLE AlgoNoDave_OpenCom( char* ComName, char* Baud, int NumBit, char Parity, int StopBit );
HANDLE AlgoNoDave_OpenSocket(const int port, const char * peer, char* all_mess);

int AlgoNoDave_ConessionePlc(int NumDispositivo, int TipoInterfaccia, int Slot, int Rec, char* all_mess);
int AlgoNoDave_DisconessionePlc( int NumDispositivo);

int AlgoNoDave_StartPlc( int NumDispositivo );
int AlgoNoDave_StopPlc ( int NumDispositivo );

int AlgoNoDave_ReadBytes  ( int NumDispositivo, int Area, int DB, int Start, int Len, void* ArrayDati, char* all_mess);
int AlgoNoDave_WritedBytes( int NumDispositivo, int Area, int DB, int Start, int Len, void* ArrayDati, char* all_mess);

int AlgoNoDave_ConvertiWORDToStringa( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali);
int AlgoNoDave_ConvertiStringaToWORD( WORD* ArrayWORD, unsigned char* ArrayCHAR, int num_canali);
int AlgoNoDave_ConvertiIntToStringa ( int* ArrayInt,   unsigned char* ArrayCHAR, int num_canali);
int AlgoNoDave_ConvertiStringaToInt ( int* ArrayInt,   unsigned char* ArrayCHAR, int num_canali);



#endif;
