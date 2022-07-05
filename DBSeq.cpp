// ------------------------------------------------------------------------
//             file       DBSEQ.CPP
// ------------------------------------------------------------------------
//
//
#include <stdio.h>
#include <mem.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <math.h>
#include <dir.h>

#include <fcntl.h>
#include <errno.h>
#include <share.h>
#include <sys\stat.h>
#include <vcl\inifiles.hpp>


#include "Libreria.h"
#include "DBSeq.h"

//==========================================================================================================
// (1)  Funzioni di servizio Timer ecc.
//==========================================================================================================

// ---------------------------
//       Tmsec()
// ---------------------------
//  Questa funzione ha lo scopo di creare la classe DBSeq ( avendo lo stesso
//  nome ). Tale funzione può non esistere, e comunque se esiste Š automati-
//  camente richiamata nella creazione (new). E' usata normalmente per
//  inzializzare variabili globali e utili alla classe.
//
Tmsec::Tmsec()
/**********/
{
  msec       = 0;
  Preset     = 0;
  Scaduto    = false;
}

// ----------------------------------------------
//  Start()
//  Inizializza il tempo con un preset in msec
// ----------------------------------------------
//
int Tmsec::Start( int MsecPreset )
{
  msec   =0;
  Scaduto=false;
  Preset = MsecPreset;
  GetLocalTime( &TimeInizio );

  return 0;
}

// ----------------------------------------------
//  Reset()
//  Reinizializza il temporizzatore
// ----------------------------------------------
//
int Tmsec::Reset( )
{
  msec   =0;
  Scaduto=false;
  GetLocalTime( &TimeInizio );

  return 0;
}

// -------------------------
//  TimeTrascorso()
// -------------------------
//   Ritorna il tempo trascorso in millisecondi
//
int Tmsec::TimeTrascorso( )
{

  GetLocalTime( &TimeFine );

  msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
  if( msec<0 ) msec+=60;
  msec = msec * 1000;
  msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);

  if( msec >= Preset ) Scaduto=true;
  if( msec <  0      ) Scaduto=true;

  return msec;
}

// -------------------------
//  TestScaduto()
// -------------------------
//   Ritorna TRUE se il tempo di Preset è scaduto
//
bool Tmsec::TestScaduto( )
{

  TimeTrascorso( );

  return Scaduto;
}


//---------------------------------------- o -----------------------------------
// ---------------------------
//       Tsec()
// ---------------------------
//  Questa funzione ha lo scopo di creare la classe DBSeq ( avendo lo stesso
//  nome ). Tale funzione può non esistere, e comunque se esiste Š automati-
//  camente richiamata nella creazione (new). E' usata normalmente per
//  inzializzare variabili globali e utili alla classe.
//
Tsec::Tsec()
/**********/
{
  sec        = 0;
  Preset     = 0;
  Scaduto    = false;
  TimeInizio = 0;
}

// ----------------------------------------------
//  Start()
//  Inizializza il tempo con un preset in msec
// ----------------------------------------------
//
int Tsec::Start( int SecPreset )
{
  sec   =0;
  Scaduto=false;
  Preset = SecPreset;

  TimeInizio = time( NULL );

  return 0;
}

// ----------------------------------------------
//  Reset()
//  Reinizializza il temporizzatore
// ----------------------------------------------
//
int Tsec::Reset( )
{
  sec   =0;
  Scaduto=false;

  TimeInizio = time( NULL );

  return 0;
}

// -------------------------
//  TimeTrascorso()
// -------------------------
//   Ritorna il tempo trascorso in millisecondi
//
int Tsec::TimeTrascorso( )
{
  sec=0;
  if(TimeInizio>0){
     TimeFine = time( NULL );
     sec = TimeFine - TimeInizio;
     if( sec >= Preset ) Scaduto=true;
     if( sec <  0      ) Scaduto=true;
  }
  return sec;
}

// -------------------------
//  TestScaduto()
// -------------------------
//   Ritorna TRUE se il tempo di Preset è scaduto
//
bool Tsec::TestScaduto( ){
  TimeTrascorso( );
  return Scaduto;
}


//---------------------------------------- o -----------------------------------

//==========================================================================================================
// (2)  Funzioni di gestione file DBSEQ
//==========================================================================================================
//---------------------------------------------------------------------------------------------------------

// ---------------------------
//       DBSeq()
// ---------------------------
//  Questa funzione ha lo scopo di creare la classe DBSeq ( avendo lo stesso
//  nome ). Tale funzione può non esistere, e comunque se esiste Š automati-
//  camente richiamata nella creazione (new). E' usata normalmente per
//  inzializzare variabili globali e utili alla classe.
//
DBSeq::DBSeq()
/**********/
{
  int i;
  CurrentDirectory( PercorsoLavoro );
  strcpy( SottoDirectory, "");
  strcpy( NomeDBase,      "");
  strcpy( PercorsoDBase,  "");
  strcpy( Estensione,  "CSV");
  CarattereSeparatore = '|';
  LenRecordImbottito  =  0;

  memset(&StrCampi, 0x00, sizeof(StrCampi));
  for( i=0; i<=DBSEQ_MAX_CAMPI; i++) PtrStrCampo[ i ] = &StrCampi[ i ][0]; // Inizializza i puntatori alle stringhe campo

  MaxLenRecord=500;

  FILEPtrFile = NULL;   // Inizializza con un valore FALSE
}



// -------------------------
//  CurrentDirectory()
// -------------------------
//
char *DBSeq::CurrentDirectory(char *path)
{
  AnsiString     Percorso;
  Percorso          = ExtractFilePath(ParamStr(0));

  strcpy( path , Percorso.c_str());
  if( strlen( path )>1 )  path[ strlen(Percorso.c_str()) -1]=0;  // Togli gli 2 ultimi caratteri  '\\'
  return (path);

}

// -------------------------
//  TimeStart()
// -------------------------
//
int DBSeq::TimeStart()
{
  GetLocalTime( &TimeInizio );
  TimeFine=TimeInizio;
  MillisecDiffTime=0;

  return 0;
}

// -------------------------
//  TimeEnd()
// -------------------------
//
int DBSeq::TimeEnd()
{
  GetLocalTime( &TimeFine );
  MillisecDiffTime=0;
  MillisecDiffTime = MillisecDiffTime + (int) ((TimeFine.wMinute - TimeInizio.wMinute)*60000);
  MillisecDiffTime = MillisecDiffTime + (int) ((TimeFine.wSecond - TimeInizio.wSecond)* 1000);
  MillisecDiffTime = MillisecDiffTime + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
  if( MillisecDiffTime <0                  ) MillisecDiffTime=0;

  return 0;
}


//---------------------------------------- o -----------------------------------


// -------------------------
//  GetCampoRecord()
// -------------------------
//
int DBSeq::GetCampoRecord( char *StringaCampo, int NumCampo, char *all_mess )
{
  int i;
  int CtrCampi=0;
  int PtrInizioCampo=0;
  int PtrFineCampo=0;
  int LenCampo=0;
  int Trovato=0;

  //-------------------------------
  // Cerca il campo nel Record
  //-------------------------------
  for( i=0; i<MaxLenRecord; i++){
     if(StringaRecord[ i ]==CarattereSeparatore || StringaRecord[ i ]==NULL){
        CtrCampi++;
        PtrFineCampo=i;
        if( CtrCampi==NumCampo ){
           LenCampo = PtrFineCampo - PtrInizioCampo;
           memcpy( StringaCampo, &StringaRecord[PtrInizioCampo], LenCampo);  // Ottimizza tempi lettura campo
           StringaCampo[LenCampo]=0;
           Trovato=1;
           break;
        }
        PtrInizioCampo=PtrFineCampo=i+1;
        if( StringaRecord[ i ]==NULL ) break;
     }
  }
  if( Trovato==0 ) { sprintf( all_mess, "Error: Field not found into Record file %s", NomeDBase ); return 1;}

  return 0;
}

// -------------------------
//  GetTuttiCampiRecord()
// -------------------------
// Viene passato un array di puntatori a caratteri
// es:
//  char Buff0[50];
//  char Buff1[50];
//  char Buff2[50];
//  ..
//  char Buff10[50];
//  char *Campi[200];
//
//  Campi[ 1]=Buff1;
//  ..
//  Campi[10]=Buff10;
//
//  TotCampi=10;
//  err= ProvaDBSEQ->GetTuttiCampiRecord( StringaRecord, Campi, TotCampi, all_mess );
//
//
int DBSeq::GetTuttiCampiRecord( int TotCampi, char *all_mess )
{
  int i;
  int CtrCampi=0;
  int PtrInizioCampo=0;
  int PtrFineCampo=0;
  int LenCampo=0;
  char StringaCampo[1000];
  bool FineCampo;

  //-------------------------------
  // Cerca il campo nel Record
  //-------------------------------
  for( i=0; i<MaxLenRecord; i++){
     FineCampo=false;
     if( StringaRecord[ i ]==NULL                 ) FineCampo=true;
     if( StringaRecord[ i ]==CarattereSeparatore  ) FineCampo=true;

     if( FineCampo ){
        CtrCampi++;
        PtrFineCampo=i;

        if( PtrFineCampo>PtrInizioCampo ){
           LenCampo = (PtrFineCampo - PtrInizioCampo);
           memcpy( StringaCampo, &StringaRecord[PtrInizioCampo], LenCampo);  // Ottimizza tempi lettura campo
           StringaCampo[LenCampo]=0;
           memcpy( PtrStrCampo[CtrCampi], StringaCampo, LenCampo+1);  // Ottimizza tempi lettura campo
        }
        PtrInizioCampo=PtrFineCampo = i+1;
        if( CtrCampi>=TotCampi ) break;
     }
     if( StringaRecord[ i ]==NULL ) break;
  }
  if( CtrCampi!=TotCampi ) { sprintf( all_mess, "Error: Field not found into Record file %s", NomeDBase ); return 1;}

  return 0;
}




// -------------------------
//  SetCampoRecord()
// -------------------------
//
int DBSeq::SetCampoRecord( char *StringaCampo, int NumCampo, char *all_mess )
{
  int PtrInizioCampo=0;
  int LenCampo=0;

  //-------------------------------
  // Compila il campo nel Record
  //-------------------------------
  if( NumCampo==1 ){
     LenCampo=strlen( StringaCampo );
     memcpy(  &StringaRecord[0], StringaCampo, LenCampo);  // Ottimizza tempi lettura campo
     StringaRecord[LenCampo]= NULL;
  }
  if( NumCampo>1 ){
     PtrInizioCampo=strlen( StringaRecord );
     StringaRecord[PtrInizioCampo]= CarattereSeparatore;
     PtrInizioCampo++;
     LenCampo=strlen( StringaCampo );
     memcpy(  &StringaRecord[PtrInizioCampo], StringaCampo, LenCampo);  // Ottimizza tempi lettura campo
     PtrInizioCampo=PtrInizioCampo+LenCampo;
     StringaRecord[PtrInizioCampo]= NULL;
  }

  return 0;
}


// -------------------------
//  SetTuttiCampiRecord()
// -------------------------
// Viene passato un array di puntatori a caratteri
// es:
//  char Buff0[50];
//  char Buff1[50];
//  char Buff2[50];
//  ..
//  char Buff10[50];
//  char *Campi[200];
//
//  Campi[ 1]=Buff1;
//  ..
//  Campi[10]=Buff10;
//
//  TotCampi=10;
//
//  err= ProvaDBSEQ->SetTuttiCampiRecord( StringaRecord, Campi, TotCampi, all_mess );
//
//
int DBSeq::SetTuttiCampiRecord( int TotCampi, char *all_mess )
{
  int CtrCampi=0;
  int PtrInizioCampo=0;
  int LenCampo=0;

  //-------------------------------
  // Compila i campi del Record
  //-------------------------------
  for( CtrCampi=1; CtrCampi<=TotCampi; CtrCampi++){
     if( CtrCampi==1 ){
        LenCampo=strlen( PtrStrCampo[CtrCampi] );
        memcpy(  &StringaRecord[0], PtrStrCampo[CtrCampi], LenCampo);  // Ottimizza tempi lettura campo
     }
     if( CtrCampi>1 ){
        StringaRecord[PtrInizioCampo]= CarattereSeparatore;
        PtrInizioCampo++;
        LenCampo=strlen( PtrStrCampo[CtrCampi] );
        memcpy(  &StringaRecord[PtrInizioCampo], PtrStrCampo[CtrCampi], LenCampo);  // Ottimizza tempi lettura campo
     }
     PtrInizioCampo=PtrInizioCampo+LenCampo;
  }
  StringaRecord[PtrInizioCampo]= NULL;

  return 0;
}


// -------------------------
//  Create()
// -------------------------
//
int DBSeq::Create( char *all_mess )
{
  sprintf( PercorsoDBase, "%s\\%s\\%s.%s", PercorsoLavoro, SottoDirectory, NomeDBase, Estensione );
  if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s\\%s.%s", PercorsoLavoro,  NomeDBase, Estensione );

  strcpy( all_mess, "");
  //-------------------------------------------
  // Crea e inizializza il file
  //-------------------------------------------
  FILEPtrFile = fopen(PercorsoDBase, "wb");
  if (FILEPtrFile<=0) { sprintf( all_mess, "Error creating file %s", NomeDBase ); return 1; }
  fclose(FILEPtrFile);

  return 0;
}


// -------------------------
//  Open()
// -------------------------
//
int DBSeq::Open( char *all_mess )
{
  sprintf( PercorsoDBase, "%s\\%s.%s", SottoDirectory, NomeDBase, Estensione );
//if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s\\%s.%s", PercorsoLavoro,  NomeDBase, Estensione );
  if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s.%s", NomeDBase, Estensione );

  strcpy( all_mess, "");
  //---------------------
  // Apre in APPEND
  //---------------------
  FILEPtrFile = fopen(PercorsoDBase, "rb+");
//FILEPtrFile = fopen(PercorsoDBase, "rb");
  if (FILEPtrFile<=0) { sprintf( all_mess, "Error Opening file %s", NomeDBase ); return 1; }

  return 0;
}

// -------------------------
//  OpenTableAppend()
// -------------------------
//
int DBSeq::OpenAppend( char *all_mess )
{
  sprintf( PercorsoDBase, "%s\\%s.%s", SottoDirectory, NomeDBase, Estensione );
//if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s\\%s.%s", PercorsoLavoro,  NomeDBase, Estensione );
  if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s.%s", NomeDBase, Estensione );

  strcpy( all_mess, "");
  //-------------------------------------------
  // Se non esiste, crea e inizializza il file
  //-------------------------------------------
  if(access(PercorsoDBase, 0)!=0){
     FILEPtrFile = fopen(PercorsoDBase, "wb+");
     if (FILEPtrFile<=0) { sprintf( all_mess, "Error Opening file %s", NomeDBase ); return 1; }
  }else{
     //---------------------
     // Apre in APPEND
     //---------------------
//     FILEPtrFile = fopen(PercorsoDBase, "ab");
     FILEPtrFile = fopen(PercorsoDBase, "rb+");
     if (FILEPtrFile<=0) { sprintf( all_mess, "Error Opening file %s", NomeDBase ); return 1; }
     fseek(FILEPtrFile, 0L, SEEK_END);                         // Posiziona alla fine del file : aggiunge 0.1msec
  }
  return 0;
}


// -------------------------
//  Delete()
// -------------------------
//
int DBSeq::Delete()
{
  sprintf( PercorsoDBase, "%s\\%s.%s", SottoDirectory, NomeDBase, Estensione );
  if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s.%s", NomeDBase, Estensione );

  //-------------------------------------------
  // Se esiste cancella il file
  //-------------------------------------------
  if(access(PercorsoDBase, 0)==0) unlink(PercorsoDBase);

  return 0;
}

// -------------------------
//  CloseTable()
// -------------------------
//
int DBSeq::Close(  )
{

  fclose(FILEPtrFile);

  return 0;
}


// -------------------------
//  WriteLine()
// -------------------------
//
int DBSeq::DBWriteLine( FILE *RFile, char *Stringa)
{
  char Stringa1[10000];
  int len;
  int LenCampo;

  //------------------------------------------------
  // Aggiunge sempre in coda i caratteri CR e LF
  //------------------------------------------------
  LenCampo=strlen( Stringa );
  memcpy( Stringa1, Stringa, LenCampo);  // Ottimizza tempi lettura campo
  Stringa1[LenCampo]   = 0x0d;
  Stringa1[LenCampo+1] = 0x0a;
  Stringa1[LenCampo+2] = NULL;

  len = fwrite( Stringa1, strlen(Stringa1), 1, RFile);  // write struct s to file
  if( len != 1 )  return 1;

  return 0;
}

// -------------------------
//  DBWriteRecord()
// -------------------------
//
int DBSeq::DBWriteRecord( char *all_mess)
{
  char Stringa1[10000];
  int LenStringa;
  int Imbottitura;
  int err=0;

  strcpy( all_mess, "");
  if( FILEPtrFile <=0 ) { sprintf( all_mess, "Invalid Pointer file %s", NomeDBase ); return 1;  }

  LenStringa=strlen( StringaRecord );
  memcpy( Stringa1, StringaRecord, LenStringa);  // Ottimizza tempi lettura campo
  //--------------------------------------------------------------------
  // Se abilitata la "IMBOTTITURA" del panino aggiunge automaticamente
  // un campo con caratteri jolly per determinare una lunghezza fissa
  //--------------------------------------------------------------------
  for( Imbottitura=LenStringa; Imbottitura<LenRecordImbottito; Imbottitura++ ){
    if( Imbottitura==LenStringa ) { Stringa1[Imbottitura]   = CarattereSeparatore; continue; }
    Stringa1[Imbottitura] = '_';
  }
  Stringa1[Imbottitura] = NULL;

  err = DBWriteLine( FILEPtrFile, Stringa1);
  if( err ) { sprintf( all_mess, "Error Writing file %s", NomeDBase ); return 1;  }

  return 0;
}

// -------------------------
//  InsertRecord()
// -------------------------
//
int DBSeq::InsertRecord( char *all_mess )
{
  int err;

  fseek(FILEPtrFile, 0L, SEEK_END);                         // Posiziona alla fine del file : aggiunge 0.1msec
                                                            // ma almeno sei sicuro sulla posizione in append
  err = DBWriteRecord( all_mess );

  return err;
}


// -------------------------
//  OpenInsertRecordClose()
// -------------------------
//
int DBSeq::OpenInsertRecordClose( char *all_mess )
{
  int err=0;

  if(err==0) err = OpenAppend(all_mess);
  if(err==0) err = InsertRecord( all_mess );
                   Close();

  return err;
}




// ------------------------------------
//  RecuperaDimensioneFile()
// ------------------------------------
// Se la funzione ritorna -1L è un errore
//
int DBSeq::RecuperaDimensioneFile( )
{
  long int PtrPosAttuale;
  long int LenFile=-1;

  PtrPosAttuale = ftell( FILEPtrFile);
  if( PtrPosAttuale>=0 ){
    fseek(FILEPtrFile, 0L, SEEK_END);                           //Posiziona alla fine del file
    LenFile = ftell( FILEPtrFile);
    fseek(FILEPtrFile, PtrPosAttuale, SEEK_SET);                //Posiziona all'INIZIO del file + Offset
    return LenFile;
  }

  return LenFile;
}


// -------------------------
//  ReadFirstRecord()
// -------------------------
//
int DBSeq::ReadFirstRecord( char *all_mess )
{
  int err;

  fseek(FILEPtrFile, 0L, SEEK_SET);                           //Posiziona all'INIZIO del file
  err = ReadNextRecord( all_mess );

  return err;
}


// -------------------------
//  ReadNextRecord()
// -------------------------
//
int DBSeq::ReadNextRecord( char *all_mess )
{
  int err;

  strcpy( all_mess, "");
  err = !Read_Line( FILEPtrFile, StringaRecord, MaxLenRecord);
  if( err ) sprintf( all_mess, "Error reading Record file %s", NomeDBase );

  return err;
}


// -------------------------
//  ReadLastRecord()
// -------------------------
//
int DBSeq::ReadLastRecord( char *all_mess )
{
  int err;
  long int PtrPosAttuale;
  long int PtrFineFile;

  PtrUltimoRecord = -1L;
  fseek(FILEPtrFile, 0L, SEEK_END);                         //Posiziona alla fine del file
  PtrFineFile = ftell( FILEPtrFile);

  if( PtrFineFile==0 ) {PtrUltimoRecord=0; return 0;}

  PtrPosAttuale=PtrFineFile-(MaxLenRecord * 3);             // Torno indietro di un pò
  if( PtrPosAttuale<0 ) PtrPosAttuale=0;

  fseek(FILEPtrFile, PtrPosAttuale, SEEK_SET);              //Posiziona a una posizione precedente
  for(;;){                                                  //Scorri gli ultimi record fino a trovare l'ultimo
     PtrUltimoRecord= PtrPosAttuale;
     err = ReadNextRecord( all_mess );
     if( err==0 )  PtrPosAttuale = ftell( FILEPtrFile)+1;
     //-------------------------------------------------------------------------
     // Se raggiungo la fine del file --> SKIP
     //-------------------------------------------------------------------------
     if( PtrPosAttuale==PtrFineFile ) break;
     if( err ) break;
  }

  //-------------------------------------------------
  // Riposiziona all'inizio dell'ultimo record !!!!
  // per eventuali sovrascritture...!!!
  //-------------------------------------------------
  fseek(FILEPtrFile, PtrUltimoRecord, SEEK_SET);              //Posiziona a una posizione precedente

  return 0;
}


// -------------------------
//  CercaRecord()
// -------------------------
//
int DBSeq::CercaRecord( int NumCampo, int Intestazione, char* ValoreCercato, int TipoRicerca, int TipoCampo, int* PosizioneRecord, char *all_mess )
{
  int err=0;
  int CtrRecord;
  char Campo[1000];
  int  Risultato;
  bool Trovato=false;

  fseek(FILEPtrFile, 0L, SEEK_SET);                           //Posiziona all'INIZIO del file
  for( CtrRecord=1; ; CtrRecord++){
     if(err==0) {err = ReadNextRecord( all_mess ); if( err ) sprintf( all_mess, "Error Record not found in file %s", NomeDBase );}
     if(err==0) {
        if(CtrRecord<=Intestazione){
           err = ReadNextRecord( all_mess );
           if( err ){sprintf( all_mess, "Error in header not found in file %s", NomeDBase ); break;}
        }
     }
     if(err==0)  err = GetCampoRecord( Campo, NumCampo, all_mess );
     if(err==0){
        if( TipoCampo==DBSEQ_VAR_STRINGA ) Risultato = strcmp( Campo, ValoreCercato );
        if( TipoCampo==DBSEQ_VAR_INTEGER ) Risultato = atoi( Campo ) - atoi( ValoreCercato) ;
        switch( TipoRicerca ){
           case DBSEQ_CONFR_MINORE       : if( Risultato<0                   ) Trovato=true; break;
           case DBSEQ_CONFR_MIN_UGUALE   : if( Risultato<0 || Risultato==0   ) Trovato=true; break;
           case DBSEQ_CONFR_UGUALE       : if( Risultato==0                  ) Trovato=true; break;
           case DBSEQ_CONFR_MAG_UGUALE   : if( Risultato>0 || Risultato==0   ) Trovato=true; break;
           case DBSEQ_CONFR_MAGGIORE     : if( Risultato>0                   ) Trovato=true; break;
           default           : break;
        }
     }
     if(Trovato==true) break;
     if( err ) break;
  }
  *PosizioneRecord=CtrRecord;
  return err;
}



// -------------------------
//  ScrollFile()
// -------------------------
//  Fa uno SCROLL di un file ASCII del numero di line rchiesto in "scroll"
//  Si può recuperare una eventuale intestazione sul file impostando
//  nel campo "intestazione" il numero di linee all'inizio del file che
//  si vogliono salvaguardare.
//
int DBSeq::ScrollFile( int intestazione, int scroll, char *all_mess)
/***************************************************************************/
{
  char buffer[1000];
  char nome_file[1000];
  int  err=0;
  FILE *ptrfile, *ptrfile1;
  int  i;

  sprintf( PercorsoDBase, "%s\\%s.%s", SottoDirectory, NomeDBase, Estensione );
  if( strlen( SottoDirectory ) ==0 )  sprintf( PercorsoDBase, "%s.%s", NomeDBase, Estensione );
  strcpy( all_mess, "");
  strcpy( nome_file, PercorsoDBase );
  //-------------------------------------------
  // Se non esiste  -> SKIP OK
  //-------------------------------------------
  if(access(PercorsoDBase, 0)!=0) return 0;

  //-------------------------------------------------------
  // Apre il file origine in lettura e l'altro in scrittura
  //-------------------------------------------------------
  ptrfile  = fopen(nome_file, "rb");
  if(ptrfile==0){ strcpy(all_mess, "Error in SCROLL_FILE()"); return 1;}
  ptrfile1 = fopen("TRANSIT.BAK", "wb");
  if(ptrfile1==0){  strcpy(all_mess, "Error in TRANSIT.BAK"); return 1;}
  //---------------------------------------------
  // Recupera eventuale "intestazione" del file
  //---------------------------------------------
  for(i=1; i<= intestazione; i++){
     err = !Read_Line(ptrfile,   buffer, MaxLenRecord);
     //if(!err) Write_Line(ptrfile1, buffer);
     if(!err) DBWriteLine( ptrfile1, buffer);
  };
  //---------------------------------------------
  // Non scrive le "Scroll" righe del  file
  //---------------------------------------------
  for(i=1; i<= scroll; i++){
     err = !Read_Line(ptrfile,   buffer, MaxLenRecord);
  }
  //---------------------------------------------
  // Riscrive la parte finale del file
  //---------------------------------------------
  while(!err){
     err = !Read_Line(ptrfile,   buffer, MaxLenRecord);
     //if(!err) Write_Line(ptrfile1, buffer);
     if(!err) DBWriteLine( ptrfile1, buffer);
  };
  fclose(ptrfile );
  fclose(ptrfile1);
  //---------------------------------------------
  // Rinomina il file provvisorio
  //---------------------------------------------
  unlink(nome_file);
  err = rename("TRANSIT.BAK", nome_file);
  if(err){ strcpy(all_mess, "ERROR IN RENAME"); return 1;}
  return 0;
}


// -------------------------
//  ScrollLongFile()
// -------------------------
//  Fa uno SCROLL di un file ASCII del numero di line rchiesto in "scroll"
//  Questa funzione è analoga a ScrollFile(), creata apposiztamente per file di
//  dimensioni esageratamente grandi > 100 Mbyte esegue la compattazione
//  in passi sucessivi:
//  a) OffsetPosition           DEVE essere =0 a inizio operazioni
//  b) NumRecordPerScrollTemp   Viene impostato con il numero di record da spostare ad ogni passo (es. 10 000 )
//  c) Con OffsetPosition==0    Viene cerato un file temporaneo con il nome del file da scrollare e estensione .TMP
//                              ad ogni passo vengono scritti 'N' record in questo file
//  d) OffsetPosition           Contiene nei passi sucessivi la posizione nel file principale
//  e) END                      Una volta raggiunta la fine del file sorgente :
//                              - Il File sorgente viene cancellato
//                              - Viene rinominato il file temporaneo
//                              - Viene Azzerato    OffsetPosition=0
//
int DBSeq::ScrollLongFile( int intestazione, int scroll, long int *OffsetPosition, int  NumRecordPerScrollTemp, char *all_mess)
/***************************************************************************/
{
  char buffer[1000];
  char nome_file[1000];
  int  err=0;
  FILE *ptrfile, *ptrfile1;
  int  i;
  char FileDiTransito[500];
  bool EndOfFile=false;
//  int  NumRecordPerScrollTemp=1000000; // Centomila

  sprintf( PercorsoDBase,  "%s\\%s.%s", SottoDirectory, NomeDBase, Estensione );
  sprintf( FileDiTransito, "%s\\%s.%s", SottoDirectory, NomeDBase, "Tmp" );
  if( strlen( SottoDirectory ) ==0 ){
     sprintf( PercorsoDBase , "%s.%s", NomeDBase, Estensione );
     sprintf( FileDiTransito, "%s.%s", NomeDBase, "Tmp" );
  }
  strcpy( all_mess, "");
  strcpy( nome_file, PercorsoDBase );
  //-------------------------------------------
  // Se non esiste  -> SKIP OK
  //-------------------------------------------
  if(access(PercorsoDBase, 0)!=0) return 0;


  //----------------------------------------------
  // (1) Primo passaggio cancella file temporaneo
  //----------------------------------------------
  if( *OffsetPosition<=0 ){     unlink(FileDiTransito);  }

  //-------------------------------------------------------
  // Apre il file origine in lettura e l'altro in scrittura
  //-------------------------------------------------------
  ptrfile  = fopen(nome_file, "rb");
  if(ptrfile==0){ strcpy(all_mess, "Error in SCROLL_FILE()"); *OffsetPosition = 0; return 1;}
  ptrfile1 = fopen(FileDiTransito, "ab+");
  if(ptrfile1==0){  sprintf(all_mess, "Error in %s", FileDiTransito); *OffsetPosition = 0; return 1;}

  //---------------------------------------------------------------
  // Se primo passaggio SCROLL
  //---------------------------------------------------------------
  if( *OffsetPosition<=0 ){
     //---------------------------------------------------------------
     // Recupera eventuale "intestazione" del file
     //---------------------------------------------------------------
     for(i=1; i<= intestazione; i++){
        err = !Read_Line(ptrfile,   buffer, MaxLenRecord);     if( err ) EndOfFile=true;
        if(!err) DBWriteLine ( ptrfile1, buffer);
     }
     //---------------------------------------------
     // Non scrive le "Scroll" righe del  file
     //---------------------------------------------
     for(i=1; i<= scroll; i++){
        err = !Read_Line(ptrfile,   buffer, MaxLenRecord);     if( err ) EndOfFile=true;
     }
  }else{
     //-------------------------------------------------------
     // Altrimenti Recupera precedente posizione di scroll
     //-------------------------------------------------------
     fseek(ptrfile, *OffsetPosition, SEEK_SET); // Nei Passaggi sucessivi recupera posizione precedente
  }
  //---------------------------------------------
  // Riscrive la parte finale del file
  //---------------------------------------------
  for( i=1; i<=NumRecordPerScrollTemp; i++){
     if( err ) break;
     if(!err ) err = !Read_Line(ptrfile,   buffer, MaxLenRecord);     if( err ) EndOfFile=true;
     if(!err ) DBWriteLine( ptrfile1, buffer);
  };
  //-----------------------------------------------------
  // (3) Se SCROLL Incompleto Salva posizione -->SKIP
  //-----------------------------------------------------
  if( err==0 ) *OffsetPosition = ftell( ptrfile );
  if( err!=0 ) *OffsetPosition = 0;

  fclose(ptrfile );
  fclose(ptrfile1);


  //-------------------------------------------------------
  // (4) Se SCROLL Incompleto Rinomina il file provvisorio
  //-------------------------------------------------------
  if( EndOfFile ){
     unlink(nome_file);
     err = rename(FileDiTransito, nome_file);
     if(err){ strcpy(all_mess, "ERROR IN RENAME"); return 1;}
  }

  return 0;
}

