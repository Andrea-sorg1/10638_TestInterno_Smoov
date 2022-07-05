//---------------------------------------------------------------------------
#ifndef ComunicaSiemens
#define ComunicaSiemens
//---------------------------------------------------------------------------

// --------------------------------
//      Variabili Globali
// --------------------------------
#ifndef MAINFILE                       // In caso non sia il MAINFILE
    #define TIPO_VARIABILE extern      // dichiararla come extern
#else
    #define TIPO_VARIABILE
#endif;

#define NUM_CICLI_RIPRISTINO_CONNESSIONE      50      // Numero cicli dopodichè azzerare il "Fine Operazione" su stazione

TIPO_VARIABILE bool SIM_PlcConnesso[110 ];           // Flag che controlla se è già attiva la connessione
//TIPO_VARIABILE int  SIM_AttesaRiconnessione[110 ];   // Attesa tentativo riconnessione (numero di cicli)


// --------------------------------
//      Funzioni
// --------------------------------

int init_porta_siemens(int dispositivo, int slot, char *all_mess);
int riconnetti_porta_siemens(int dispositivo, int slot, char *all_mess);

int SIM_ConnectToPlc  ( int NumDispositivo, int TipoCom, int NumCom, char* Baudreate, char* IpAddress, int TcpPort, int Slot, int Rec, char* all_mess);
int SIM_RiconnectToPlc( int NumDispositivo, int TipoCom, int NumCom, char* Baudreate, char* IpAddress, int TcpPort, int Slot, int Rec, char* all_mess);

int SIM_S5ReadWordPlc ( int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess);
int SIM_S5WriteWordPlc( int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess);

int  SIM_S7ReadBytePlcSiemens ( int NumDispositivo, int TipoArea, int  NumDB,  int  NrByteIni, int NumByte, BYTE * DataByte, char* all_mess);
int  SIM_S7WriteBytePlcSiemens( int NumDispositivo, int TipoArea, int  NumDB,  int  NrByteIni, int NumByte, BYTE * DataByte, char* all_mess);
int  SIM_S7ReadDBSiemens      ( int NumDispositivo, int  NumDB,  int NrByteIni, int LenDb, BYTE * DataByte, char* all_mess);
int  SIM_S7WriteDBSiemens     ( int NumDispositivo, int  NumDB,  int NrByteIni, int LenDb, BYTE * DataByte, char* all_mess);

int SIM_S7ReadWordPlc ( int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess);
int SIM_S7WriteWordPlc( int NumDispositivo, int Area, int NumDB, int NrDatoIni, int NrDati, WORD* WordDati, char* all_mess);




#endif
