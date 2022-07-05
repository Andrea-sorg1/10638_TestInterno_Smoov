// -----------------------------------------------------------------------
//         file        AlgoDde.CPP
// -----------------------------------------------------------------------
#include <vcl.h>
#include <errno.h>
#include <stdio.h>


#include <ddeml.h>

#include "AlgoDde.h"


//------------------------------
//     Variabili
//------------------------------
ALGO_DDE AlgoDde;
char AlgoDde_StringaProva          [ 200 ];
char AlgoDde_StringaExecute        [ 200 ];
bool AlgoDde_ServerAbilitato=1;


//------------------------------
//     Funzioni
//------------------------------


//-------------------------------------
//  AlgoDde_Callback()
//-------------------------------------
HDDEDATA CALLBACK AlgoDde_Callback(
    UINT uType,	      // transaction type
    UINT uFmt,	      // clipboard data format
    HCONV hconv,	      // handle to the conversation
    HSZ hsz1,	         // handle to a string
    HSZ hsz2,	         // handle to a string
    HDDEDATA hdata,	   // handle to a global memory object
    DWORD dwData1,  	// transaction-specific data
    DWORD dwData2 	   // transaction-specific data
   ){
  char Stringa1[300], Stringa2[300];
  HDDEDATA HandleDato;
  int err=0;
  LPBYTE PtrData;
  int   LenData;
  int   DataLenRicevuto;

  DdeQueryString( AlgoDde.IdInstanzaDDE, hsz1, Stringa1, sizeof( Stringa1 ), 0);
  DdeQueryString( AlgoDde.IdInstanzaDDE, hsz2, Stringa2, sizeof( Stringa2 ), 0);


  //---------------------------------------------
  // Lato SERVER
  //---------------------------------------------
  switch( uType){
     //---------------------------------
     // Richiesta CONNESSIONE
     //---------------------------------
     case XTYP_CONNECT:
        if( AlgoDde_ServerAbilitato==false ) return ( HDDEDATA) 0; 
        AlgoDde_ServerElaboraComandoRicevuto( XTYP_CONNECT, hdata, Stringa2, PtrData, &LenData);
        if( strcmp( Stringa2, AlgoDde.ServerLocale )==0 ){ AlgoDde.ClientRemotoConnesso=true; return ( HDDEDATA) 1; }
     break;

     //---------------------------------
     // Richiesta CONNESSIONE
     //---------------------------------
     case XTYP_DISCONNECT:
        AlgoDde.ClientRemotoConnesso=false;
        AlgoDde_ServerElaboraComandoRicevuto( XTYP_DISCONNECT, hdata, Stringa2, PtrData, &LenData);
//        if( strcmp( Stringa1, "" )==0 && strcmp( Stringa2, ""  )==0 ){ AlgoDde.ServerRemotoConnesso=false; return ( HDDEDATA) 1; }
     break;

     //------------------------------------
     // RICEZIONE DATI -> SERVER EXECUTE
     //------------------------------------
     case XTYP_EXECUTE:
        //-----------------------------------------------------------------------
        // Verifica il comando ricevuto e se OK copia il dato sulla variabile
        //-----------------------------------------------------------------------
        DataLenRicevuto = DdeGetData ( hdata, NULL, 0, 0 );   // Così si ricavare la sola lunghezza del dato !!!!
        DdeGetData( hdata, (LPBYTE) AlgoDde_StringaExecute,  sizeof(AlgoDde_StringaExecute), 0 );
        err = AlgoDde_ServerElaboraComandoRicevuto( XTYP_EXECUTE, hdata, AlgoDde_StringaExecute, PtrData, &LenData);
        if( err==0  ) return ( HDDEDATA) DDE_FACK;
        if( err     ) return ( HDDEDATA) 0;  // Ritorna errore  !!!
     break;
     //-------------------------------------
     // RICEZIONE DATI -> SERVER POKE
     //-------------------------------------
     case XTYP_POKE:
        //-----------------------------------------------------------------------
        // Verifica il comando ricevuto e se OK copia il dato sulla variabile
        //-----------------------------------------------------------------------
        err = AlgoDde_ServerElaboraComandoRicevuto( XTYP_POKE, hdata, Stringa2, PtrData, &LenData);
        if( err==0  ){
           DataLenRicevuto = DdeGetData ( hdata, NULL, 0, 0 );   // Così si ricavare la sola lunghezza del dato !!!!
           //DdeGetData( hdata, (LPBYTE) PtrData,  LenData, 0 );
           return ( HDDEDATA) DDE_FACK;
        }
        if( err )  return ( HDDEDATA) 0;  // Ritorna errore  !!!
     break;
     //---------------------------------
     // RICHIESTA DATI <- SERVER
     //---------------------------------
     case XTYP_REQUEST:
        //-----------------------------------------------------------------------
        // Verifica il comando ricevuto e se OK copia il dato sulla variabile
        //-----------------------------------------------------------------------
        err = AlgoDde_ServerElaboraComandoRicevuto( XTYP_REQUEST, hdata, Stringa2, PtrData, &LenData);
        if( err==0  ){
           //HandleDato = DdeCreateDataHandle( AlgoDde.IdInstanzaDDE, (LPBYTE) PtrData, LenData, 0, hsz2, CF_TEXT, 0);
           HandleDato = DdeCreateDataHandle( AlgoDde.IdInstanzaDDE, (LPBYTE) PtrData, LenData, 0, hsz2, uFmt, 0);
           return HandleDato;
        }
        if( err )  return ( HDDEDATA) 0;  // Ritorna errore  !!!
     break;

     default:
     break;
  }


  DataLenRicevuto=DataLenRicevuto;  // Potrebbe essere utile in futuro per controlli
  return (HDDEDATA) 0;
}
//---------------------------------------------------------------------------



//-------------------------------------
//  AlgoDde_Initialize()
//-------------------------------------
int  AlgoDde_Initialize(char* NomeServerLocale)
{
  HSZ hsz1;
  int i;

  AlgoDde.IdInstanzaDDE=0;
  memset( &AlgoDde, 0, sizeof( AlgoDde ));
  for( i=0; i<=DDE_MAX_CLIENT; i++ ) AlgoDde.cl[ i ].TimeOut=200;     // Inizializza il TimeOut di default
  //------------------------------------------------
  // Iniazializzazione DDE Dynamic Data Exchange
  //------------------------------------------------
  strcpy( AlgoDde.SoggettoConversazione, "AlgoDdeDati" );     // Sempre fisso: Il flusso dei dati lo discriminiamo con
                                                              // Nome del Server del Servizio e Nome del ITEM (variabile)
  DdeInitialize( &AlgoDde.IdInstanzaDDE, AlgoDde_Callback, 0, 0 );

  //-----------------------------------------------------------------------------------
  // Volendo essere anche DDE SERVER occorre inizializzare il nome del "SERVIZIO" DDE
  // al quale si vuole rispondere
  // Il nome del Servizio di solito coincide con il nome dell'applicazione
  // ma potrebbe essere qualsiasi
  //-----------------------------------------------------------------------------------
  strcpy( AlgoDde.ServerLocale, NomeServerLocale );
  hsz1 = DdeCreateStringHandle( AlgoDde.IdInstanzaDDE, AlgoDde.ServerLocale,  0);
  DdeNameService( AlgoDde.IdInstanzaDDE, hsz1, NULL, DNS_REGISTER );
  DdeFreeStringHandle( AlgoDde.IdInstanzaDDE, hsz1 ); // Libera sempre gli Handle creati

  return 0;
}
//---------------------------------------------------------------------------


//-------------------------------------
//  AlgoDde_Initialize()
//-------------------------------------
int  AlgoDde_Uninitialize()
{
  DdeUninitialize( AlgoDde.IdInstanzaDDE );  return 0;
}
//---------------------------------------------------------------------------

//-------------------------------------
//  AlgoDde_ClientConnection()
//-------------------------------------
int  AlgoDde_ClientConnection( int NumClient, char *ServerConnessione, char* all_mess)
{
   HSZ   hszService, hszTopic;

   if( NumClient>DDE_MAX_CLIENT ){ sprintf( all_mess,  "Num Client Out of Range 1..%d.", DDE_MAX_CLIENT );  return 1;}
   //------------------------------------------
   // Richiesta di un SERVIZIO e un SOGGETTO
   //------------------------------------------
   strcpy( AlgoDde.cl[NumClient].ServerRemoto,     ServerConnessione);

   hszService = DdeCreateStringHandle( AlgoDde.IdInstanzaDDE, AlgoDde.cl[NumClient].ServerRemoto,  0);
   hszTopic   = DdeCreateStringHandle( AlgoDde.IdInstanzaDDE, AlgoDde.SoggettoConversazione,  0);

   AlgoDde.cl[NumClient].hConv      = DdeConnect( AlgoDde.IdInstanzaDDE, hszService, hszTopic,  0);

   DdeFreeStringHandle( AlgoDde.IdInstanzaDDE, hszService ); // Libera sempre gli Handle creati
   DdeFreeStringHandle( AlgoDde.IdInstanzaDDE, hszTopic   ); // Libera sempre gli Handle creati
   //--------------------------------
   // Se conessione NON stabilita
   //--------------------------------

   if( !AlgoDde.cl[NumClient].hConv ){
     AlgoDde.cl[NumClient].ServerRemotoConnesso= false;
     sprintf( all_mess,  "Could not link to DDE server: %s.",AlgoDde.cl[NumClient].ServerRemoto );  return 1;
   }
   AlgoDde.cl[NumClient].ServerRemotoConnesso= true;   return 0;
}
//---------------------------------------------------------------------------




//-------------------------------------
//  AlgoDde_ClientDisconnect()
//-------------------------------------
int  AlgoDde_ClientDisconnect( int NumClient, char* all_mess )
{

  if( NumClient>DDE_MAX_CLIENT ){ sprintf( all_mess,  "Num Client Out of Range 1..%d.", DDE_MAX_CLIENT );  return 1;}
  if( AlgoDde.cl[NumClient].ServerRemotoConnesso ){
     DdeDisconnect( AlgoDde.cl[NumClient].hConv );
     AlgoDde.cl[NumClient].ServerRemotoConnesso=false;
     AlgoDde.cl[NumClient].hConv = NULL;
  }

  return 0;
}
//---------------------------------------------------------------------------

//-------------------------------------
//  AlgoDde_TransactionError()
//-------------------------------------
int  AlgoDde_TransactionError( int NumClient, char* all_mess)
{
  UINT DdeError;
  char Server[100];

  strcpy( Server, AlgoDde.cl[ NumClient ].ServerRemoto);
  DdeError = DdeGetLastError( AlgoDde.IdInstanzaDDE );

  switch ( DdeError )  {
     case DMLERR_ADVACKTIMEOUT:       sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_ADVACKTIMEOUT",         Server, DdeError );   break;
     case DMLERR_BUSY:                sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_BUSY",                  Server, DdeError );   break;
     case DMLERR_DATAACKTIMEOUT:      sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_DATAACKTIMEOUT",        Server, DdeError );   break;
     case DMLERR_DLL_NOT_INITIALIZED: sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_DLL_NOT_INITIALIZED",   Server, DdeError );   break;
     case DMLERR_DLL_USAGE:           sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_DLL_USAGE",             Server, DdeError );   break;
     case DMLERR_EXECACKTIMEOUT:      sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_EXECACKTIMEOUT",        Server, DdeError );   break;
     case DMLERR_INVALIDPARAMETER:    sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_INVALIDPARAMETER",      Server, DdeError );   break;
     case DMLERR_LOW_MEMORY:          sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_LOW_MEMORY",            Server, DdeError );   break;
     case DMLERR_MEMORY_ERROR:        sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_MEMORY_ERROR",          Server, DdeError );   break;
     case DMLERR_NO_CONV_ESTABLISHED: sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_NO_CONV_ESTABLISHED",   Server, DdeError );   break;
     case DMLERR_NOTPROCESSED:        sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_NOTPROCESSED",          Server, DdeError );   break;
     case DMLERR_POKEACKTIMEOUT:      sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_POKEACKTIMEOUT",        Server, DdeError );   break;
     case DMLERR_POSTMSG_FAILED:      sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_POSTMSG_FAILED",        Server, DdeError );   break;
     case DMLERR_REENTRANCY:          sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_REENTRANCY",            Server, DdeError );   break;
     case DMLERR_SERVER_DIED:         sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_SERVER_DIED",           Server, DdeError );   break;
     case DMLERR_SYS_ERROR:           sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_SYS_ERROR",             Server, DdeError );   break;
     case DMLERR_UNADVACKTIMEOUT:     sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_UNADVACKTIMEOUT",       Server, DdeError );   break;
     case DMLERR_UNFOUND_QUEUE_ID:    sprintf( all_mess, "Server:'%s' DdeError %X DMLERR_UNFOUND_QUEUE_ID",      Server, DdeError );   break;
     default:                         sprintf( all_mess, "Server:'%s' DdeError %X UNKNOW Error",                 Server, DdeError );   break;
  }
  return  DdeError;
}
//---------------------------------------------------------------------------
//-------------------------------------
//  AlgoDde_ClientExecute()
//-------------------------------------
int  AlgoDde_ClientExecute( int NumClient, char* Dato, DWORD LenDato,  char* all_mess)
{
  HDDEDATA Result, HandleDato;
  DWORD dwTimeout= AlgoDde.cl[ NumClient ].TimeOut;
  int err=0;
  char Buffer[200];

  if( NumClient>DDE_MAX_CLIENT ){ sprintf( all_mess,  "Num Client Out of Range 1..%d.", DDE_MAX_CLIENT );  return 1;}
  if( !AlgoDde.cl[NumClient].ServerRemotoConnesso ) { sprintf( all_mess, "Server :%s NOT Connect",  AlgoDde.cl[NumClient].ServerRemoto ); return 1;};
  //----------------------------------------------
  // Transaction    EXECUTE
  //----------------------------------------------
  HandleDato = DdeCreateDataHandle( AlgoDde.IdInstanzaDDE, (LPBYTE) Dato, LenDato, 0, NULL, CF_TEXT, 0);
  if( HandleDato!=0 ){
     //Result = DdeClientTransaction( ( LPBYTE ) Dato, (DWORD) LenDato, HCONV hConv, HSZ hszItem, UINT wFmt, UINT wType, DWORD dwTimeout, NULL);
     Result = DdeClientTransaction( ( LPBYTE ) HandleDato, 0xFFFFFFFF, AlgoDde.cl[NumClient].hConv, 0L, 0, XTYP_EXECUTE, dwTimeout, NULL);
  }

  DdeFreeDataHandle( HandleDato );  // Libera sempre gli Handle creati
  //----------------------------------------------
  // Controllo del risultato
  //----------------------------------------------
  //if( !Result ) {AlgoDde_TransactionError( all_mess );  return 1;}
  if( !Result ) { err=AlgoDde_TransactionError( NumClient, all_mess );  if(err==DMLERR_INVALIDPARAMETER) AlgoDde_ClientDisconnect( NumClient, Buffer ); return 1;}
  DdeFreeDataHandle( Result );                       // Libera sempre gli Handle creati

  return 0;
}
//---------------------------------------------------------------------------


//-------------------------------------
//  AlgoDde_ClientPoke()
//-------------------------------------
int  AlgoDde_ClientPoke( int NumClient, char* Item, char* Dato, DWORD LenDato,  char* all_mess)
{
  HDDEDATA Result, HandleDato;  HSZ hszItem;	   // handle to item name string
  DWORD dwTimeout= AlgoDde.cl[ NumClient ].TimeOut;
  int err=0;
  char Buffer[200];

  if( NumClient>DDE_MAX_CLIENT ){ sprintf( all_mess,  "Num Client Out of Range 1..%d.", DDE_MAX_CLIENT );  return 1;}
  if( !AlgoDde.cl[NumClient].ServerRemotoConnesso ) { sprintf( all_mess, "Server :%s NOT Connect",  AlgoDde.cl[NumClient].ServerRemoto ); return 1;};
  //----------------------------------------------
  // Transaction    POKE
  //----------------------------------------------
  hszItem    = DdeCreateStringHandle( AlgoDde.IdInstanzaDDE, Item,  0);
  HandleDato = DdeCreateDataHandle( AlgoDde.IdInstanzaDDE, (LPBYTE) Dato, LenDato, 0, Item, CF_TEXT, 0);

  //Result = DdeClientTransaction( ( LPBYTE ) Dato, (DWORD) LenDato, HCONV hConv, HSZ hszItem, UINT wFmt, UINT wType, DWORD dwTimeout, NULL);
  Result = DdeClientTransaction( ( LPBYTE ) HandleDato, -1, AlgoDde.cl[NumClient].hConv, hszItem, CF_TEXT, XTYP_POKE, dwTimeout, NULL);

  DdeFreeStringHandle( AlgoDde.IdInstanzaDDE, hszItem ); // Libera sempre gli Handle creati
  DdeFreeDataHandle( HandleDato );                       // Libera sempre gli Handle creati

  //----------------------------------------------
  //Esempio di POKE utilizzando uno String Handle
  //HSZ hszDato    = DdeCreateStringHandle( AlgoDde.IdInstanzaDDE, Dato,  0);
  //Result = DdeClientTransaction( ( LPBYTE ) Dato, (DWORD) LenDato, AlgoDde.hConv, hszItem, CF_TEXT, XTYP_POKE, dwTimeout, NULL);
  //DdeFreeStringHandle( AlgoDde.IdInstanzaDDE, hszDato ); // Libera sempre gli Handle creati
  //----------------------------------------------
  //----------------------------------------------
  // Controllo del risultato
  //----------------------------------------------
  //if( !Result ) {AlgoDde_TransactionError( all_mess );  return 1;}
  if( !Result ) { err=AlgoDde_TransactionError( NumClient, all_mess );  if(err==DMLERR_INVALIDPARAMETER) AlgoDde_ClientDisconnect( NumClient, Buffer ); return 1;}
  DdeFreeDataHandle( Result );                       // Libera sempre gli Handle creati

  return 0;
}
//---------------------------------------------------------------------------


//-------------------------------------
//  AlgoDde_ClientRequest()
//-------------------------------------
int  AlgoDde_ClientRequest( int NumClient, char* Item, char* Dato, DWORD LenDato,  char* all_mess)
{
  HDDEDATA Result, HandleDato;  HSZ hszItem;	   // handle to item name string
  DWORD dwTimeout= AlgoDde.cl[ NumClient ].TimeOut;
  DWORD   data_len;
  char  * pData;
  int err=0;
  char Buffer[200];

  if( NumClient>DDE_MAX_CLIENT ){ sprintf( all_mess,  "Num Client Out of Range 1..%d.", DDE_MAX_CLIENT );  return 1;}
  if( !AlgoDde.cl[NumClient].ServerRemotoConnesso ) { sprintf( all_mess, "Server :%s NOT Connect",  AlgoDde.cl[NumClient].ServerRemoto ); return 1;};
  //----------------------------------------------
  // Transaction    POKE
  //----------------------------------------------
  hszItem    = DdeCreateStringHandle( AlgoDde.IdInstanzaDDE, Item,  0);
  HandleDato = DdeCreateDataHandle( AlgoDde.IdInstanzaDDE, (LPBYTE) Dato, LenDato, 0, Item, CF_TEXT, 0);

  //Result = DdeClientTransaction( ( LPBYTE ) Dato, (DWORD) LenDato, HCONV hConv, HSZ hszItem, UINT wFmt, UINT wType, DWORD dwTimeout, NULL);
  Result = DdeClientTransaction( NULL, 0, AlgoDde.cl[NumClient].hConv,  hszItem, CF_TEXT, XTYP_REQUEST, dwTimeout,  NULL);

  DdeFreeStringHandle( AlgoDde.IdInstanzaDDE, hszItem ); // Libera sempre gli Handle creati
  DdeFreeDataHandle( HandleDato );                       // Libera sempre gli Handle creati

  //----------------------------------------------
  // Controllo del risultato
  //----------------------------------------------
  //if( !Result ) {AlgoDde_TransactionError( all_mess );  return 1;}
  if( !Result ) { err=AlgoDde_TransactionError( NumClient, all_mess );  if(err==DMLERR_INVALIDPARAMETER) AlgoDde_ClientDisconnect( NumClient, Buffer ); return 1;}

  pData= (char*) DdeAccessData( Result, &data_len );
  memcpy( Dato, pData, LenDato );  // Copia solo LenDato Byte che dovrebbe essere la dimensione di Dato
  DdeUnaccessData( Result );
  DdeFreeDataHandle( Result );                       // Libera sempre gli Handle creati


  return 0;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------
// Funzione da implementare nei moduli della appliczaione
//---------------------------------------------------------
//-----------------------------------------------------
//  AlgoDde_ServerElaboraComandoRicevuto()
// Ritorna 0 se OK con Ptr e Lungheza della variabile
//-----------------------------------------------------
/*
int  AlgoDde_ServerElaboraComandoRicevuto( UINT TipoComando, HDDEDATA hdata, char* Comando, LPBYTE &PtrVariabile, int* LenVariabile)
{
  int err=1;


  //-------------------------------------
  // CLIENT -> SERVER  ( EXECUTE )
  //-------------------------------------
  if( TipoComando==XTYP_EXECUTE ){
     if( strcmp( Comando, "Prova EXECUTE")==0 ){strcpy( AlgoDde_StringaProva, AlgoDde_StringaExecute); err=0;}
     //....
     //....
  }

  //-------------------------------------
  // CLIENT -> SERVER  ( POKE )
  //-------------------------------------
  if( TipoComando==XTYP_POKE ){
     if( strcmp( Comando, "WRITE_PROVA"    )==0 ){  PtrVariabile=(LPBYTE) AlgoDde_StringaProva; *LenVariabile= sizeof(AlgoDde_StringaProva); err=0;}
     //....
     //....
     DdeGetData( hdata, (LPBYTE) PtrVariabile,  *LenVariabile, 0 );
  }

  //-------------------------------------
  // CLIENT <- SERVER ( REQUEST )
  //-------------------------------------
  if( TipoComando==XTYP_REQUEST ){
     if( strcmp( Comando, "READ_PROVA"    )==0 ){ PtrVariabile= (LPBYTE )RispostaRequestProva; *LenVariabile= sizeof(RispostaRequestProva); err=0;}
     //....
     //....
  }


  return err;
}
//---------------------------------------------------------------------------
*/
