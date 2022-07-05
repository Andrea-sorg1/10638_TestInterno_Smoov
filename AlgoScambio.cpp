// ---------------------------------------------------------------------------
//                     file     ALGOSCAMBIO.CPP
//
//  N.B. Questo MODULO viene mantenuto uguale sia per l'applicazione
//       BOX2000 che per quella SERVER COMUNICAZIONI per facilitare la
//       manutenzione.
// ------------------------------------------------------------------------
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>
#include <io.h>
#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>
#include "Main.h"
#include "OmronETH.h"
#include "AlgoScambio.h"
#include "DBSeq.h"
#include "Variabil.h"
#include "AlgoDde.h"



// ------------------------
//    Scambio_Inizializza()
// ------------------------
//  Inizializzazione variabili
//
void Scambio_Inizializza()
// ***********************************
{
  AnsiString  TempProjectIniFileName;

  //-----------------------------------------------------
  // Creazione della directory di scambio
  // Se NON esiste crea anche la directory WINDOWS
  // ( In alcuni sistemi è WINNT )
  //-----------------------------------------------------
  mkdir("C:\\WINDOWS");
  mkdir("C:\\WINDOWS\\ALGOSISTEMI");

  strcpy(  Scambio_PercorsoScambio ,   "C:\\WINDOWS\\ALGOSISTEMI" );
  sprintf( Scambio_FileNameAreaDM_RD,  "%s\\%s", Scambio_PercorsoScambio, "DM_RD.ALG" );
  sprintf( Scambio_FileNameAreaDM_WD,  "%s\\%s", Scambio_PercorsoScambio, "DM_WD.ALG" );
  sprintf( Scambio_FileNameServerIni,  "%s\\%s", Scambio_PercorsoScambio, "ServerComunicazioni.ini" );
  TempProjectIniFileName = LowerCase(ExtractFileName(Application->ExeName));
  TempProjectIniFileName = TempProjectIniFileName.SubString(1, TempProjectIniFileName.Length() - 3) + "ini";
  Copy_File( TempProjectIniFileName.c_str(), Scambio_FileNameServerIni);

  memset(&Scambio_Area   , 0, sizeof(AreaScambioPlc));
  memset(&Scambio_AreaRD , 0, sizeof(AreaScambioPlc));
  memset(&Scambio_AreaWD , 0, sizeof(AreaScambioPlc));

  Scambio_InizializzaTag();

  return ;
}


// --------------------------
//   Scambio_LoadFileDM_RD()
// --------------------------
//   load file DM PLCTERRA
//
int Scambio_LoadFileDM_RD(char *all_mess)
// ***********************************
{
  int  i,
       err =0;
  int  handle;


  memset(&Scambio_AreaRD , 0, sizeof(AreaScambioPlc));
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     unlink( Scambio_FileNameAreaDM_RD );
     return 0;
  }
  //---------------------
  // Apertura file
  //---------------------
  handle = sopen( Scambio_FileNameAreaDM_RD, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      //close( handle );
      sprintf(all_mess, "Error in load file: %s", Scambio_FileNameAreaDM_RD);
      return 1;
  }
  i = read( handle, &Scambio_AreaRD, sizeof( AreaScambioPlc ) );
  close( handle );

  if (i != sizeof(Scambio_AreaRD)){
    memset(&Scambio_AreaRD , 0, sizeof(AreaScambioPlc));
    err = 1;
  }
  //if(err) sprintf(all_mess, "Errore Lettura File: %s", Scambio_FileNameAreaDM);

  return err;
}


// ----------------------------
//   Scambio_SaveFileDM_RD()
// ----------------------------
//   salva file DM PLCTERRA
//
int Scambio_SaveFileDM_RD(char *all_mess)
//***********************************
{
  int  i,
       err = 0;
  int  handle;


  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     unlink( Scambio_FileNameAreaDM_RD );
     return 0;
  }
  //---------------------
  // Apertura file
  //---------------------
  handle = sopen( Scambio_FileNameAreaDM_RD, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
      sprintf(all_mess, "Error in write file: %s", Scambio_FileNameAreaDM_RD);
      //close( handle );
      return 1;
  }

  //------------------------------------------------------
  // Aggiornamento versione DATI per scambio PC - SERVER
  //------------------------------------------------------
  Scambio_AreaRD.VersioneDati = VERSIONE_DATI;

  i =  write( handle, &Scambio_AreaRD, sizeof( AreaScambioPlc ) );
  close( handle );
  if (i != sizeof(AreaScambioPlc))  err = 1;
  if(err) sprintf(all_mess, "Error write File: %s", Scambio_FileNameAreaDM_RD);
  return err;
}



// --------------------------
//   Scambio_LoadFileDM_WD()
// --------------------------
//   load file DM PLCTERRA
//
int Scambio_LoadFileDM_WD(char *all_mess)
// ***********************************
{
  int  i,
       err =0;
  int  handle;


  memset(&Scambio_AreaWD , 0, sizeof(AreaScambioPlc));
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){ 
     unlink( Scambio_FileNameAreaDM_WD );
     return 0;
  }
  //---------------------
  // Apertura file
  //---------------------
  handle = sopen( Scambio_FileNameAreaDM_WD, O_RDWR | O_BINARY, SH_DENYNO );
  if( handle<0 ){
      //close( handle );
      sprintf(all_mess, "Error in load file: %s", Scambio_FileNameAreaDM_WD);
      return 1;
  }
  i = read( handle, &Scambio_AreaWD, sizeof( AreaScambioPlc ) );
  close( handle );

  if (i != sizeof(Scambio_AreaWD)){
    memset(&Scambio_AreaWD , 0, sizeof(AreaScambioPlc));
    err = 1;
  }
  //if(err) sprintf(all_mess, "Errore Lettura File: %s", Scambio_FileNameAreaDM);

  return err;
}


// ----------------------------
//   Scambio_SaveFileDM_WD()
// ----------------------------
//   salva file DM PLCTERRA
//
int Scambio_SaveFileDM_WD(char *all_mess)
//***********************************
{
  int  i,
       err = 0;
  int  handle;

  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     unlink( Scambio_FileNameAreaDM_WD );
     return 0;
  }
  //---------------------
  // Apertura file
  //---------------------
  handle = sopen( Scambio_FileNameAreaDM_WD, O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
  if( handle<0 ){
      sprintf(all_mess, "Error in write file: %s", Scambio_FileNameAreaDM_WD);
      //close( handle );
      return 1;
  }
  //------------------------------------------------------
  // Aggiornamento versione DATI per scambio PC - SERVER
  //------------------------------------------------------
  Scambio_AreaWD.VersioneDati = VERSIONE_DATI;
  i =  write( handle, &Scambio_AreaWD, sizeof( AreaScambioPlc ) );
  close( handle );

  if (i != sizeof(AreaScambioPlc))  err = 1;
  if(err) sprintf(all_mess, "Error in write File: %s", Scambio_FileNameAreaDM_RD);
  return err;
}

//------------------------------   o   -------------------------------------------------
//-------------------------------------------------------------------------------------
// Funzioni per SUPERVISORE   BOX2000
//-------------------------------------------------------------------------------------
// ----------------------------------------
// (1) Scambio_SV_LetturaInizioProgramma()
// ----------------------------------------
//
int Scambio_SV_LetturaInizioProgramma(char *all_mess)
//***********************************
{
  int err;
  char buffer[200];

  err=0;
  sprintf( all_mess, "" );


  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 VECCHIA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde==0 ){
     //-------------------------------------------
     // Recupero ultima situazione salvata
     //-------------------------------------------
     err = Scambio_LoadFileDM_WD( all_mess );
     if( err==0 ) memcpy( &Scambio_Area, &Scambio_AreaWD, sizeof(AreaScambioPlc));
  }
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     ScambioDdeSV_LetturaDatiGenerali( buffer );
  }

  //-----------------------------------------------------------------
  // All'accensione Forzo il progressivo a un valore molto superiore
  // al precedente spegnimento: il SERVER deve rispondere con un valore
  // coerente per potersi ritenere valido (SINCRONIZZATO)
  //-----------------------------------------------------------------
  Scambio_Area.Progressivo = Scambio_Area.Progressivo+100;

  return err;
}

// ----------------------------------
// (2) Scambio_SV_LetturaInizioCiclo()
// ----------------------------------
//   Lettura file DM PLCTERRA
//
int Scambio_SV_LetturaInizioCiclo(char *all_mess)
//***********************************
{
  int err;
  int i;

  err=0;
  sprintf( all_mess, "" );
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 VECCHIA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde==0 ){
     //-------------------------------------------
     // Leggo DM da SERVER se dati attendibili
     //-------------------------------------------
     err = Scambio_LoadFileDM_RD( all_mess );
     Scambio_Area.DatiAttendibili = Scambio_AreaRD.DatiAttendibili;
     Scambio_Area.VersioneDati    = Scambio_AreaRD.VersioneDati;

     if( err ) Scambio_Area.DatiAttendibili=-1;
     if(( Scambio_Area.Progressivo   - Scambio_AreaRD.Progressivo) > 10 ) Scambio_Area.DatiAttendibili=0;
     if(( Scambio_AreaRD.Progressivo - Scambio_Area.Progressivo  ) > 10 ) Scambio_Area.DatiAttendibili=0;
     //---------------------------------------------
     // Salvo Dati in AREA DM LOCALE
     //---------------------------------------------
     for( i=0; i<=MAX_AREA_DM_PLCTERRA; i++){
        //-----------------------------------------------------------------
        // Se FLAG Mod del DM READ == FLAG Mod del DM locale (WRITE)
        // allora il SERVER COMUNICAZIONI ha recepito il comando scrittura
        // e il DM è valido e azzero il FALG di modificato
        // mantengo valore vecchio e incremento
        //-----------------------------------------------------------------
        if( Scambio_AreaRD.DM_Mod [ i ] == Scambio_Area.DM_Mod [ i ] ){
           Scambio_Area.DM     [ i ]     = Scambio_AreaRD.DM      [ i ];
           Scambio_Area.DM_Mod [ i ]     = 0;
        }
     }
  }
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     unlink( Scambio_FileNameAreaDM_RD );
     err = ScambioDdeSV_LetturaDatiGenerali( all_mess);
  }


  return err;
}



// ------------------------------------------
// (3) Scambio_ld_canale()
// ------------------------------------------
// Legge una sequenza di canali per un massimo di 20000 canali.
// n.b. questa funzione ha significato solo per comunicazioni Ethernet
//
WORD Scambio_ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  WORD err;
  int i;

  err=0;
  sprintf( all_mess, "" );

  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 VECCHIA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde==0 ){
     //-------------------------------------------
     // Controllo DATI richiesti
     //-------------------------------------------
     if(strcmp(cmd, "RD")!=0)                                 err=1;
     if( plc!=0 )                                             err=2;
     if( canale_ini             >MAX_AREA_DM_PLCTERRA    )    err=3;
     if( canale_ini+num_canali  >MAX_AREA_DM_PLCTERRA    )    err=4;
     if( err ) { sprintf( all_mess, "WRONG Command ERR%d", err);  return err; }


     //------------------------------------------------------------------
     // Se il il file di scambio NON è leggibile -> DatiAttendibili = -1
     //------------------------------------------------------------------
     if( Scambio_Area.DatiAttendibili<0 ){
        err= 97;
        sprintf( all_mess, "Err=%d FILE DATA EXCHANGE SERVER COMUNICATION PLCTERRA ERROR", err);
        return err;
     }
     //------------------------------------------------------------
     // Se la versione dei dati di scambio NON coincide con
     // quella del SERVER --> ERR
     //------------------------------------------------------------
     if(Scambio_Area.VersioneDati   != VERSIONE_DATI ){
        err= 98;
        sprintf( all_mess, "Err=%d SERVER COMUNICATION DATA EXCHANGE VERSION IS DIFFERENT %d - %d ", err, VERSIONE_DATI, Scambio_Area.VersioneDati );
        return err;
     }

     //------------------------------------------------------------
     // Se il Server è spento o non restituisce dati validi -->ERR
     //------------------------------------------------------------
     if( Scambio_Area.DatiAttendibili==0 ){
        err= 99;
        sprintf( all_mess, "Err=%d SERVER COMUNICATION PLCTERRA OFF (or comunication PLC error)", err);
        return err;
     }
     //---------------------------------------------
     // Recupero Dati da AREA DM LOCALE
     //---------------------------------------------
     for( i=1; i<=num_canali; i++){
        //--------------------------------------------------
        // Se FLAG Modificato=1 DM <- AREA WRITE
        //--------------------------------------------------
        canale[i-1] = Scambio_Area.DM [ canale_ini+i-1 ];
     }
  }
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     err = ScambioDdeSV_ld_canale( canale, plc, cmd, canale_ini, num_canali, all_mess);
  }

  return err;
}


// ------------------------------------------
// (4) Scambio_wr_canale()
// ------------------------------------------
// Scrive una sequenza di canali per un massimo di 700 canali.
// n.b. questa funzione ha significato solo per comunicazioni Ethernet
//
WORD Scambio_wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  WORD err;
  int i;

  err=0;
  sprintf( all_mess, "" );
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 VECCHIA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde==0 ){
     //-------------------------------------------
     // Controllo DATI richiesti
     //-------------------------------------------
     if(strcmp(cmd, "WD")!=0)                                 err=1;
     if( plc!=0 )                                             err=2;
     if( canale_ini             >MAX_AREA_DM_PLCTERRA    )    err=3;
     if( canale_ini+num_canali  >MAX_AREA_DM_PLCTERRA    )    err=4;
     if( err ) { sprintf( all_mess, "WRONG Command ERR%d", err);  return err; }

     //---------------------------------------------
     // Salvo Dati in AREA DM LOCALE
     //---------------------------------------------
     for( i=1; i<=num_canali; i++){
        //--------------------------------------------------
        // Se FLAG Modificato=1 DM <- AREA WRITE
        //--------------------------------------------------
        Scambio_Area.DM      [ canale_ini+i-1 ] = canale[i-1];
        //------------------------------------------------------------------------
        // Se Risponde con FLAG MOD==0 Forzo 1 altrimenti incremento fino MAX 100
        //------------------------------------------------------------------------
        if( Scambio_AreaRD.DM_Mod  [ canale_ini+i-1 ] == 0 ||
            Scambio_AreaRD.DM_Mod  [ canale_ini+i-1 ] >= 100 ){
            Scambio_Area.DM_Mod  [ canale_ini+i-1 ] =1;   // Forza FLAG di MODIFICATO
        }else{
           Scambio_Area.DM_Mod  [ canale_ini+i-1 ] == Scambio_AreaRD.DM_Mod  [ canale_ini+i-1 ] +1  ;   // Forza FLAG di MODIFICATO
        }
     }
  }

  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     err = ScambioDdeSV_wr_canale( canale, plc, cmd, canale_ini, num_canali, all_mess);
  }

  return err;
}




// ------------------------------------
//  (5) Scambio_SV_ScritturaFineCiclo()
// ------------------------------------
//   salva file DM PLCTERRA
//
int Scambio_SV_ScritturaFineCiclo(char *all_mess)
//***********************************
{
  int err;

  err=0;
  sprintf( all_mess, "" );

  //---------------------------------------------
  // Aggiorna DATI e SALVA
  //---------------------------------------------
  Scambio_Area.Progressivo++;
  if( Scambio_Area.Progressivo>1000000 ) Scambio_Area.Progressivo=1;

  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 VECCHIA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde==0 ){
     memcpy( &Scambio_AreaWD, &Scambio_Area, sizeof(AreaScambioPlc));
     err = Scambio_SaveFileDM_WD( all_mess );
  }
  //----------------------------------------------------
  // AlgoDdeX 12/10/2006 NUOVA Modalità SCAMBIO DATI &&&&&&&&&&&&&&&&&&&&&&&&
  //----------------------------------------------------
  if( AlgoScambioModalitaDde ){
     unlink( Scambio_FileNameAreaDM_WD );
     err = ScambioDdeSV_ScritturaDatiGenerali( all_mess);
  }


  return err;
}




//----------------------------------------  O ------------------------------------
//---------------------------------------------------------
//         Gestione TAG
//---------------------------------------------------------
// ------------------------
//    Scambio_Inizializza()
// ------------------------
//  Inizializzazione variabili
//
void Scambio_InizializzaTag()
// ***********************************
{
  //-----------------------------------------------------
  // Creazione del percorso file
  //-----------------------------------------------------
  sprintf( Scambio_FileNameTag,        "%s\\%s", Scambio_PercorsoScambio, "TAG.CSV" );
  sprintf( Scambio_FileNameTagBak,     "%s\\%s", Scambio_PercorsoScambio, "TAG.BAK" );

  memset(&Scambio_Tag   , 0, sizeof(Scambio_Tag));

  //----------------------------------------------
  // Cancellazione del file .BAK a inizio CICLO
  //----------------------------------------------
  unlink( Scambio_FileNameTagBak );

  return ;
}



// ------------------------------------
//  int StatMacInizializzaTabellaCSV();
// ------------------------------------
// Inizializza i parametri della tabella
//
void Scambio_InizializzaTabellaTagCsv(char *path, DBSeq* DBSeqTabella)
{
  sprintf( DBSeqTabella->SottoDirectory,   path );
  sprintf( DBSeqTabella->Estensione,       "CSV");
  sprintf( DBSeqTabella->NomeDBase,        "TAG" );
  DBSeqTabella->CarattereSeparatore      = ';';
  DBSeqTabella->TipoFile                 = DBSEQ_TIPO_FILE_STANDARD;

  return;
}



//---------------------------------------------------------------------------
// ----------------------------------------
//  void Scambio_SetCampiTabellaTagCSV()
// ----------------------------------------
// Copia i campi della struttura Record sui campi omonimi della TTABLE
//
void Scambio_SetCampiTabellaTagCSV(int IndiceTag, struct AreaTag *Record, DBSeq *Tabella, char* all_mess)
{
  int  TotCampi=MAX_CAMPI_TAG;

  //---------------------------------------------
  // Compila i campi del record
  //---------------------------------------------
  sprintf( Tabella->StrCampi[ 1 ], "%d", IndiceTag );
  sprintf( Tabella->StrCampi[ 2 ], "%d", Record->TipoComando );
  sprintf( Tabella->StrCampi[ 3 ], "%d", Record->DMIni );
  sprintf( Tabella->StrCampi[ 4 ], "%d", Record->NumDM  );
  sprintf( Tabella->StrCampi[ 5 ], "%d", Record->IndicePlcReale   );
  sprintf( Tabella->StrCampi[ 6 ], "%d", Record->DMIniPlcReale  );
  sprintf( Tabella->StrCampi[ 7 ], "%d", Record->Aux6 );
  sprintf( Tabella->StrCampi[ 8 ], "%d", Record->Aux7 );
  sprintf( Tabella->StrCampi[ 9 ], "%d", Record->Aux8 );
  sprintf( Tabella->StrCampi[10 ], "%d", Record->Aux9 );
  sprintf( Tabella->StrCampi[11 ], "%d", Record->Aux10);

  Tabella->SetTuttiCampiRecord( TotCampi, all_mess );

  return;
}

//---------------------------------------------------------------------------
// ---------------------------------------
//  void Scambio_GetCampiTabellaTagCSV()
// ---------------------------------------
// Copia i campi della TTABLE  sui campi omonimi della struttura Record
//
void Scambio_GetCampiTabellaTagCSV(int IndiceTag, struct AreaTag *Record, DBSeq *Tabella, char* all_mess)
{
  int  TotCampi=MAX_CAMPI_TAG;

  Tabella->GetTuttiCampiRecord( TotCampi, all_mess );
  //------------------------------
  // Compila struttura dei Record
  //------------------------------

  Record->TipoComando     = atoi( Tabella->StrCampi[ 2] );
  Record->DMIni           = atoi( Tabella->StrCampi[ 3] );
  Record->NumDM           = atoi( Tabella->StrCampi[ 4] );
  Record->IndicePlcReale  = atoi( Tabella->StrCampi[ 5] );
  Record->DMIniPlcReale   = atoi( Tabella->StrCampi[ 6] );
  Record->Aux6            = atoi( Tabella->StrCampi[ 7] );
  Record->Aux7            = atoi( Tabella->StrCampi[ 8] );
  Record->Aux8            = atoi( Tabella->StrCampi[ 9] );
  Record->Aux9            = atoi( Tabella->StrCampi[10] );
  Record->Aux10           = atoi( Tabella->StrCampi[11] );

  return;
}


//---------------------------------------------------------------------------
// ---------------------------------------
//  int Scambio_WriteTabellaTagCSV()
// ---------------------------------------
// Copia i campi della TTABLE  sui campi omonimi della struttura Record
//
int Scambio_WriteTabellaTagCSV( char* all_mess)
{
  int  NumTag;
  int  ErrDB=0;

  //--------------------------------------------
  // Cancella Tabella
  //--------------------------------------------
  DBSeq *NewTable = new DBSeq;
  //if( NewTable->Create(all_mess) ) { delete NewTable; return 1; }
  unlink( Scambio_FileNameTag );
  //-----------------------------------------------------
  // Apertura Tabella
  //-----------------------------------------------------
  if( ErrDB==0 ){
     Scambio_InizializzaTabellaTagCsv( Scambio_PercorsoScambio, NewTable);
     ErrDB = NewTable->OpenAppend( all_mess );   // APRI TABELLA IN APPEND ( se NON esiste la CREA )
     if( ErrDB==0 ){
        //----------------------------------------------------------
        // Compila intestazione (SOLO ai fini di VISUALIZZAZIONE)
        //----------------------------------------------------------
        sprintf( NewTable->StrCampi[1], "Indice" );
        sprintf( NewTable->StrCampi[2], "TipoComando" );
        sprintf( NewTable->StrCampi[3], "DmIni" );
        sprintf( NewTable->StrCampi[4], "NumDM" );
        sprintf( NewTable->StrCampi[5], "IndicePlcReale" );
        sprintf( NewTable->StrCampi[6], "DMIniPlcReale" );
        NewTable->SetTuttiCampiRecord( 6, all_mess );
        ErrDB = NewTable->InsertRecord( all_mess );                 // Insert RECORD alla posizione di END OF FILE
        if( ErrDB==0 ){
           //---------------------------------------
           // Compila Tabella
           //---------------------------------------
           for( NumTag=1; NumTag<MAX_TAG; NumTag++ ){
              Scambio_SetCampiTabellaTagCSV( NumTag, &Scambio_Tag.T[ NumTag], NewTable, all_mess);
              ErrDB = NewTable->InsertRecord( all_mess );                 // Insert RECORD alla posizione di END OF FILE
              if( ErrDB ) break;
              //             if( NumTag>0 && Scambio_Tag.T[ NumTag ].NumDM==0 ) break;
           }
        }
     }
  }
  NewTable->Close();
  delete NewTable;

  //--------------------------------------------
  // Cancella Tabella BAK
  //--------------------------------------------
  if( ErrDB==0 ) unlink( Scambio_FileNameTagBak );

  return ErrDB;
}



//---------------------------------------------------------------------------
// ---------------------------------------
//  int Scambio_WriteTabellaTagCSV()
// ---------------------------------------
// Copia i campi della TTABLE  sui campi omonimi della struttura Record
//
int Scambio_LoadTabellaTagCSV( char* all_mess)
{
  int  NumTag;
  int  ErrDB=0;
  int  ErrDB1=0;


  //--------------------------------------------
  // Ottimizzazione:
  // Se File TAG insesistente ->SKIP
  // Se File BAK esistente    ->SKIP
  //--------------------------------------------
  if(access( Scambio_FileNameTag,    0)!=0) return 0;
  if(access( Scambio_FileNameTagBak, 0)==0) return 0;

  //--------------------------------------------
  // Cancella Tabella
  //--------------------------------------------
  DBSeq *NewTable = new DBSeq;
  //-----------------------------------------------------
  // Apertura Tabella
  //-----------------------------------------------------
  if( ErrDB==0 ){
     Scambio_InizializzaTabellaTagCsv( Scambio_PercorsoScambio, NewTable);
     ErrDB = NewTable->Open( all_mess );   // APRI TABELLA IN APPEND ( se NON esiste la CREA )
     if( ErrDB==0 ){
        //---------------------------------------
        // Elimina 1° Record di Intestazione
        //---------------------------------------
        ErrDB = NewTable->ReadNextRecord( all_mess );   // scorrimento della LISTA
        if( ErrDB==0 ){
           //---------------------------------------
           // Recupera Tabella
           //---------------------------------------
           for( NumTag=1; NumTag<MAX_TAG; NumTag++ ){
              ErrDB1 = NewTable->ReadNextRecord( all_mess );   // scorrimento della LISTA
              if( ErrDB1==0 ) Scambio_GetCampiTabellaTagCSV( NumTag, &Scambio_Tag.T[ NumTag], NewTable, all_mess);    // recuper
              if( ErrDB1 ) break;
              //             if( NumTag>0 && Scambio_Tag.T[ NumTag ].NumDM==0 ) break;
           }
        }
     }
  }
  NewTable->Close();
  delete NewTable;

  //--------------------------------------------
  // Copia del file --> BAK
  //--------------------------------------------
  if( ErrDB==0 ) Copy_File( Scambio_FileNameTag, Scambio_FileNameTagBak);

  return ErrDB;
}







//----------------------------------------  O ------------------------------------
//---------------------------------------------------------
//         Gestione SCAMBIO DATI TRAMITE DDE
//---------------------------------------------------------

DatiScambioDdeGeneraliR   DdeGeneraliR;
DatiScambioDdeGeneraliW   DdeGeneraliW;
DatiScambioDdeCanali      DdeCanali;

int AlgoScambioModalitaDde  =0;
int AlgoScambioTrsDdeInCorso=0;



// --------------------------------------------
//  (2A)   ScambioDdeSV_LetturaDatiGenerali()
// --------------------------------------------
int ScambioDdeSV_LetturaDatiGenerali( char* all_mess)
{
  int err=0;

  //--------------------------------
  // Se Server Remoto NON Connesso
  //--------------------------------
  if( !AlgoDde.cl[1].ServerRemotoConnesso )  AlgoDde_ClientConnection( 1, "Servercomunicazioni", all_mess );

  AlgoScambioTrsDdeInCorso=1;
  //--------------------------------
  // Solo se DDE Abilitato
  //--------------------------------
  if( AlgoScambioModalitaDde==1 ){
     err = AlgoDde_ClientRequest( 1, "READ_SCAMBIO_DATI_GENERALI", (char *) &DdeGeneraliR, (DWORD) sizeof(DdeGeneraliR), all_mess);
     if( err==0 ){
        Scambio_Area.VersioneDati            = DdeGeneraliR.VersioneDati;
        Scambio_Area.DatiAttendibili         = DdeGeneraliR.DatiAttendibili;
        memcpy( Scambio_Area.ComunicazioneOk,  DdeGeneraliR.ComunicazioneOk, sizeof(Scambio_Area.ComunicazioneOk) ) ;
        memcpy( Scambio_Area.Messaggio,        DdeGeneraliR.Messaggio,       sizeof(Scambio_Area.Messaggio) ) ;
     }
  }
  AlgoScambioTrsDdeInCorso=0;
  return err;
}



// ----------------------------------------------
//  (5A)   ScambioDdeSV_ScritturaDatiGenerali()
// ----------------------------------------------
int ScambioDdeSV_ScritturaDatiGenerali( char* all_mess)
{
  int err=0;

  //--------------------------------
  // Se Server Remoto NON Connesso
  //--------------------------------
  //if( !AlgoDde.cl[1].ServerRemotoConnesso ) { if( AlgoDde_ClientConnection( 1, "Servercomunicazioni", all_mess )==0)  AlgoScambioModalitaDde=1; }
  if( !AlgoDde.cl[1].ServerRemotoConnesso )  AlgoDde_ClientConnection( 1, "Servercomunicazioni", all_mess );

  AlgoScambioTrsDdeInCorso=1;
  //--------------------------------
  // Solo se DDE Abilitato
  //--------------------------------
  if( AlgoScambioModalitaDde==1 ){
     DdeGeneraliW.TotMacchine    = Scambio_Area.TotMacchine;
     DdeGeneraliW.TotAgv         = Scambio_Area.TotAgv;
     DdeGeneraliW.Configurazione = Scambio_Area.Configurazione;
     DdeGeneraliW.Progressivo    = Scambio_Area.Progressivo;
     memcpy( DdeGeneraliW.MacchinaDiCarico, Scambio_Area.MacchinaDiCarico, sizeof(Scambio_Area.MacchinaDiCarico) ) ;

     err = AlgoDde_ClientPoke( 1, "WRITE_SCAMBIO_DATI_GENERALI", (char *) &DdeGeneraliW, (DWORD) sizeof(DdeGeneraliW), all_mess);
  }
  AlgoScambioTrsDdeInCorso=0;

  return err;
}





// ------------------------------------------
// (3A) ScambioDdeSV_ld_canale()
// ------------------------------------------
// Legge una sequenza di canali per un massimo di 1000 canali.
// n.b. questa funzione ha significato solo per comunicazioni Ethernet
//
WORD ScambioDdeSV_ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  int  err;
  char Buffer[200];

  err=0;
  sprintf( all_mess, "" );
  //-------------------------------------------
  // Controllo DATI richiesti
  //-------------------------------------------
  if(strcmp(cmd, "RD")!=0)                                 err=1;
  if( plc!=0 )                                             err=2;
  if( canale_ini             >MAX_AREA_DM_PLCTERRA    )    err=3;
  if( canale_ini+num_canali  >MAX_AREA_DM_PLCTERRA    )    err=4;
  if( num_canali             >MAX_CANALI_SCAMBIO_DDE  )    err=5;
  if( err ) { sprintf( all_mess, "WRONG Command ERR%d", err);  return (WORD) err; }

  //---------------------------------------------
  // Recupero Dati da AREA DM SERVER
  //---------------------------------------------
  AlgoScambioTrsDdeInCorso=1;
  sprintf( Buffer, "READ_DM        |PLC=%2d|INI=%5d|LEN=%5d|", plc, canale_ini, num_canali );
  err = AlgoDde_ClientRequest( 1, Buffer, (char *) &DdeCanali, (DWORD) sizeof(DdeCanali), all_mess);
  if( err==0 ){
     if( DdeCanali.err    ) {strcpy( all_mess, DdeCanali.Messaggio ); err=DdeCanali.err+10;}
     if( DdeCanali.err==0 )  memcpy( canale, DdeCanali.Dm, num_canali*sizeof(WORD)  );
  }
  AlgoScambioTrsDdeInCorso=0;

  return (WORD) err;
}




// ------------------------------------------
// (4A) ScambioDdeSV_wr_canale()
// ------------------------------------------
// Scrive una sequenza di canali per un massimo di 1000 canali.
//
WORD ScambioDdeSV_wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  int  err;
  char Buffer[200];

  err=0;
  sprintf( all_mess, "" );
  //-------------------------------------------
  // Controllo DATI richiesti
  //-------------------------------------------
  if(strcmp(cmd, "WD")!=0)                                 err=1;
  if( plc!=0 )                                             err=2;
  if( canale_ini             >MAX_AREA_DM_PLCTERRA    )    err=3;
  if( canale_ini+num_canali  >MAX_AREA_DM_PLCTERRA    )    err=4;
  if( num_canali             >MAX_CANALI_SCAMBIO_DDE  )    err=5;
  if( err ) { sprintf( all_mess, "WRONG Command ERR%d", err);  return (WORD) err; }

  //---------------------------------------------
  // Salvo Dati in AREA DM SERVER
  //---------------------------------------------
  memset( &DdeCanali, 0, sizeof( DdeCanali ));
  memcpy( DdeCanali.Dm, canale, num_canali*sizeof(WORD)  );

  AlgoScambioTrsDdeInCorso=1;
  sprintf( Buffer, "WRITE_DM       |PLC=%2d|INI=%5d|LEN=%5d|", plc, canale_ini, num_canali );
  err = AlgoDde_ClientPoke   ( 1, Buffer, (char *) &DdeCanali, (DWORD) sizeof(DdeCanali)   , all_mess);
  if( err==0 &&  DdeCanali.err    ) {strcpy( all_mess, DdeCanali.Messaggio ); err=DdeCanali.err+10;}
  AlgoScambioTrsDdeInCorso=0;

  return (WORD) err;
}



//-----------------------------------------------------
//  AlgoDde_ServerElaboraComandoRicevuto()
//
// Ritorna 0 se OK con Ptr e Lungheza della variabile
//-----------------------------------------------------
int  AlgoDde_ServerElaboraComandoRicevuto( UINT TipoComando, HDDEDATA hdata, char* Comando, LPBYTE &PtrVariabile, int* LenVariabile)
{
  int err=1;
  char IntestazioneComando[200];
  char Buffer[200];
  int  plc, canale_ini, num_canali;


  //---------------------------------------------------------------
  // Scomposizione Comandi strutturati
  //---------------------------------------------------------------
  //                  1234567890123456789012234567890123456789012
  //sprintf( Buffer, "READ_DM        |PLC=%2d|INI=%  5d|LEN=%  5d|", plc, canale_ini, num_canali );
  strcpy( IntestazioneComando, Comando); IntestazioneComando[15]=NULL;
  Copy( Comando, 21, 2, Buffer );  plc         =atoi(Buffer);
  Copy( Comando, 28, 5, Buffer );  canale_ini  =atoi(Buffer);
  Copy( Comando, 38, 5, Buffer );  num_canali  =atoi(Buffer);
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
     if( strcmp( Comando,             "WRITE_PROVA"                 )==0 ){ PtrVariabile=(LPBYTE) AlgoDde_StringaProva; *LenVariabile= sizeof(AlgoDde_StringaProva); err=0;}
     //--------------------------------------------------------
     // Scrittura DATI GENERALI
     //--------------------------------------------------------
     if( strcmp( Comando,             "WRITE_SCAMBIO_DATI_GENERALI" )==0 ){ PtrVariabile=(LPBYTE) &DdeGeneraliW;        *LenVariabile= sizeof(DdeGeneraliW); err=0;
        if( err==0 ){
           DdeGetData( hdata, (LPBYTE) PtrVariabile,  *LenVariabile, 0 );

           Scambio_Area.TotMacchine      = DdeGeneraliW.TotMacchine;
           Scambio_Area.TotAgv           = DdeGeneraliW.TotAgv;
           Scambio_Area.Configurazione   = DdeGeneraliW.Configurazione;
           memcpy( Scambio_Area.MacchinaDiCarico, DdeGeneraliW.MacchinaDiCarico, sizeof(Scambio_Area.MacchinaDiCarico) ) ;
        }
     }
     //--------------------------------------------------------
     // Scrittura DM
     //--------------------------------------------------------
     if( strcmp( IntestazioneComando, "WRITE_DM       "           )==0 ){ PtrVariabile=(LPBYTE) &DdeCanali;           *LenVariabile= sizeof(DdeCanali);  err=0;
        if( plc!=0 )                                             err=2;
        if( canale_ini             >MAX_AREA_DM_PLCTERRA    )    err=3;
        if( canale_ini+num_canali  >MAX_AREA_DM_PLCTERRA    )    err=4;
        if( num_canali             >MAX_CANALI_SCAMBIO_DDE  )    err=5;
        if( err    ){ sprintf( DdeCanali.Messaggio, "WRONG Command ERR%d", err); DdeCanali.err=err;}
        if( err==0 ){
           DdeGetData( hdata, (LPBYTE) PtrVariabile,  *LenVariabile, 0 );

           memcpy( &Scambio_Area.DM    [ canale_ini ], DdeCanali.Dm, num_canali*sizeof(WORD)  );
           memset( &Scambio_Area.DM_Mod[ canale_ini ], 1, num_canali );
        }
     }
     //....
     //....
  }

  //-------------------------------------
  // CLIENT <- SERVER ( REQUEST )
  //-------------------------------------
  if( TipoComando==XTYP_REQUEST ){
     if( strcmp( Comando, "READ_PROVA"    )==0 )               { PtrVariabile= (LPBYTE )AlgoDde_StringaProva; *LenVariabile= sizeof(AlgoDde_StringaProva); err=0;}
     //--------------------------------------------------------
     // Lettura DATI GENERALI
     //--------------------------------------------------------
     if( strcmp( Comando,             "READ_SCAMBIO_DATI_GENERALI" )==0 ){ PtrVariabile=(LPBYTE) &DdeGeneraliR;        *LenVariabile= sizeof(DdeGeneraliR); err=0;
        if( err==0 ){
           DdeGeneraliR.VersioneDati     = Scambio_Area.VersioneDati;
           DdeGeneraliR.DatiAttendibili  = Scambio_Area.DatiAttendibili;
           memcpy( DdeGeneraliR.ComunicazioneOk, Scambio_Area.ComunicazioneOk,  sizeof(Scambio_Area.ComunicazioneOk) ) ;
           memcpy( DdeGeneraliR.Messaggio, Scambio_Area.Messaggio, sizeof(Scambio_Area.Messaggio) ) ;
        }
     }
     //--------------------------------------------------------
     // Lettura DM
     //--------------------------------------------------------
     if( strcmp( IntestazioneComando, "READ_DM        "           )==0 ){ PtrVariabile=(LPBYTE) &DdeCanali;           *LenVariabile= sizeof(DdeCanali);  err=0;
        if( plc!=0 )                                             err=2;
        if( canale_ini             >MAX_AREA_DM_PLCTERRA    )    err=3;
        if( canale_ini+num_canali  >MAX_AREA_DM_PLCTERRA    )    err=4;
        if( num_canali             >MAX_CANALI_SCAMBIO_DDE  )    err=5;
        if( err    ){ sprintf( DdeCanali.Messaggio, "WRONG Command ERR%d", err); DdeCanali.err=err;}
        if( Scambio_Area.DatiAttendibili<=0) {  err=6;  sprintf( DdeCanali.Messaggio, "SERVER DDE Data Not available err%d", err);DdeCanali.err=err;}
        if( err==0 ){
           memset( &DdeCanali, 0, sizeof(DdeCanali) );
           memcpy( DdeCanali.Dm, &Scambio_Area.DM    [ canale_ini ], num_canali*sizeof(WORD)  );
        }
     }
     //....
     //....
  }


  return err;
}
//---------------------------------------------------------------------------


