//---------------------------------------------------------------------------
#ifndef AlgoDdeH
#define AlgoDdeH

#include <ddeml.h>

#define DDE_MAX_CLIENT 10
//---------------------------------------------------------------------------

struct ALGO_CLI{
  //---------------------------------------------------
  // Relative alla connessione (Conversazione) CLIENT
  //---------------------------------------------------
  HCONV hConv;                       // Handle a una connessione/Conversation
  DWORD TimeOut;                     // TimeOut della transaction sincrona ( per trans. asincrona TIMEOUT_ASYNC)

  bool  ServerRemotoConnesso;
  bool  ClientRemotoConnesso;
  char  ServerRemoto  [50];          // Nome del Server Remoto del SEVIZIO richiesto

};


struct ALGO_DDE{
  //-------------------------------------
  // Relative alla istanza DDE
  //-------------------------------------
  DWORD IdInstanzaDDE;
  char  ServerLocale  [30];          // Nome del Server Locale del SEVIZIO richiesto
  char  SoggettoConversazione[30];   // Sempre fisso: "AlgoDdeDati"
                                     // Il flusso dei dati lo discriminiamo con
                                     // Nome del Server del Servizio e Nome del ITEM (variabile)
  bool  ClientRemotoConnesso;        // Lato SERVER se un client è connesso
  //-------------------------------------------
  // Relative alla connessione (Conversazione)
  //-------------------------------------------
  ALGO_CLI cl[DDE_MAX_CLIENT+1];     // Struccura per diversi CLIENT

};
extern ALGO_DDE AlgoDde;

extern char AlgoDde_StringaExecute       [ 200 ];
extern char AlgoDde_StringaProva         [ 200 ];
extern bool AlgoDde_ServerAbilitato;

//-------------------------------------
//  Funzioni
//-------------------------------------
int  AlgoDde_Initialize(char* NomeServerLocale);
int  AlgoDde_Uninitialize();

int  AlgoDde_ClientConnection( int NumClient, char *ServerConnessione, char* all_mess);
int  AlgoDde_ClientDisconnect( int NumClient, char* all_mess );

int  AlgoDde_TransactionError( int NumClient, char* all_mess);
int  AlgoDde_ClientExecute( int NumClient,             char* Dato, DWORD LenDato,  char* all_mess);
int  AlgoDde_ClientPoke   ( int NumClient, char* Item, char* Dato, DWORD LenDato,  char* all_mess);
int  AlgoDde_ClientRequest( int NumClient, char* Item, char* Dato, DWORD LenDato,  char* all_mess);

int  AlgoDde_ServerElaboraComandoRicevuto( UINT TipoComando, HDDEDATA hdata, char* Comando, LPBYTE &PtrVariabile, int* LenVariabile);


#endif;

