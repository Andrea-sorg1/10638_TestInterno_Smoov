// ------------------------------------------------------------------------
//                              file AllenBViaVbOpc.CPP
// ------------------------------------------------------------------------
//
//-------------------------------------------
// Elenco Funzioni incluse nel file
//-------------------------------------------
//-------------------------------------------
#include <mem.h>
#include <string.h>
#include <math.h>
#include <share.h>
#include <sys\stat.h>

#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <vcl.h>
#include <vcl\inifiles.hpp>


//#include "Tecnofer.h"
//#include "Sistema.h"
#include "Libreria.h"
#include "AllenBViaVbOpc.h"
#include "WSocket.hpp"
#include "Slc500.h"

//-------------------------------------  o   ---------------------------------


// -----------------------------------
//    AllenB_Inizializza()
// -----------------------------------
//
int AllenB_Inizializza( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  int err=0;
  int Plc;

  CicloPlcAllen=0;
  strcpy( all_mess, "" );
  AllenB_Percorso();
  //--------------------------------------------------
  // - Inizializza Dati
  //--------------------------------------------------


  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[ Plc ].Indice = Plc;
     DatiPLC[ Plc ].R.DatiOk =0;
     DatiPLC[ Plc ].CtrErroriConsecutivi=0;
     DatiPLC[ Plc ].ErrorePersistente=false;
     DatiPLC[ Plc ].DisableComunicazione=false;
  }
  //-------------------------------------------------
  // Cancella file dati letti
  //-------------------------------------------------
  sprintf( NomeFile, "r_file");
  sprintf( FileTemp, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );

  sprintf( NomeFile, "DataRead");
  sprintf( FileTemp, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );

  sprintf( NomeFile, "SemaforoDataRead");
  sprintf( FileTemp, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );

  sprintf( NomeFile, "SemaforoDataWrite");
  sprintf( FileTemp, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );


  Slc500_InitComunicazioni( );

  return err;
}


// -----------------------------------
//    AllenB_InizializzaFileConfig()
// -----------------------------------
//
int AllenB_InizializzaFileConfig( char* all_mess )
// *********************************
{
  if( DatiPLC[ 0 ].TipoComunicazione==1 ){
     AllenB_MAS_InitRead_ScritturaFile( all_mess );
     AllenB_MAS_InitWrite_ScritturaFile( all_mess );
  }
  if( DatiPLC[ 0 ].TipoComunicazione==2 ){
     AllenB_LUC_CompilazioneFileTagCFG( all_mess );
     AllenB_LUC_CompilazioneFileOpcCFG( all_mess );
     AllenB_LUC_InitRead_ScritturaFile( all_mess );
  }

  return 0;
}


// -----------------------------------
//    AllenB_Percorso()
// -----------------------------------
//
void AllenB_Percorso( )
// *********************************
{
  strcpy( PercorsoAllenB , PathGateway.c_str());
}




// ------------------------------
//  AllenB_GetTuttiCampiRecord()
// ------------------------------
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
//
int AllenB_GetTuttiCampiRecord( int TotCampiReq, int *TotCampiRead, char* StringaRecord, char CarattereSeparatore, char *all_mess )
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
  for( i=0; i<MAX_LEN_RECORD; i++){
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
        if( CtrCampi>=TotCampiReq ) break;
     }
     if( StringaRecord[ i ]==NULL ) break;
  }
  *TotCampiRead=CtrCampi;
  //  if( CtrCampi!=TotCampi ) { sprintf( all_mess, "Error: Field not found into Record file %s", NomeDBase ); return 1;}

  return 0;
}

//-------------------------------
//   AllenB_ReadIniProject()
//-------------------------------
// Lettura del file *.ini di sistema
//
void AllenB_ReadIniProject()
{

  TIniFile *IniFile;
  AnsiString Buff;
  AnsiString FIniFileName;
  char         Session[50];
  char         Buffer[300];
  char         Buffer1[300];
  int i;
  AnsiString    PathProjectAllen;


  //-------------------------------------------
  // Recupero il path del progetto
  //-------------------------------------------
  PathProjectAllen = ExtractFilePath(ParamStr(0));
  FIniFileName = PathProjectAllen + "AllenB.ini";

  //FIniFileName = "AllenB.ini";
  IniFile      = new TIniFile(FIniFileName);
  //-----------------------------------------------------
  // GENERALI
  //-----------------------------------------------------
  sprintf( Session, "GENERALI");
  PathGateway    = IniFile->ReadString (Session, "PathGateway", PathGateway);
  DatiPLC[0].TipoComunicazione = IniFile->ReadInteger(Session, "TipoComunicazione", 1);
  AllenbPlcIni = IniFile->ReadInteger(Session, "AllenbPlcIni", 1);
  AllenbTotPlc = IniFile->ReadInteger(Session, "AllenbTotPlc", 1);
  if( AllenbTotPlc>MAX_PLC_AB ) AllenbTotPlc=MAX_PLC_AB;

  //-----------------------------------------------------
  // PLC
  //-----------------------------------------------------
  for( i=1; i<=MAX_PLC_AB; i++ ){
     sprintf( Session, "PLC%02d", i );
     DatiPLC[i].Indice = i;
     sprintf( Buffer, "[PLC%02d]", i );
     Buff = IniFile->ReadString (Session, "Topic", Buffer);         strcpy( DatiPLC[i].Topic,  Buff.c_str());
     Buff = IniFile->ReadString (Session, "IpAddr","192.168.1.60"); strcpy( DatiPLC[i].IpAddr, Buff.c_str());
     DatiPLC[i].Port      = IniFile->ReadInteger(Session, "Port", 2222);

     Buff = IniFile->ReadString (Session, "RItem",  "N7");    strcpy( DatiPLC[i].R.Item,  Buff.c_str());
     DatiPLC[i].R.DatoIni      = IniFile->ReadInteger(Session, "RDatoIni", 1);
     DatiPLC[i].R.NumDati      = IniFile->ReadInteger(Session, "RNumDati", 1);

     Buff = IniFile->ReadString (Session, "WItem",  "N7");     strcpy( DatiPLC[i].W.Item,  Buff.c_str());
     DatiPLC[i].W.DatoIni      = IniFile->ReadInteger(Session, "WDatoIni", 1);
     DatiPLC[i].W.NumDati      = IniFile->ReadInteger(Session, "WNumDati", 1);
  }


  delete IniFile;

}
//---------------------------------------------------------------------------
//-------------------------------
//   AllenB_WriteIniProject()
//-------------------------------
// Scrittura del file *.ini di sistema
//
void AllenB_WriteIniProject()
{
  TIniFile *IniFile;
  AnsiString Buff;
  char         Session[50];
  char         Buffer[300];
  char         Buffer1[300];
  int i;

  char         stringa[201]="";
  AnsiString   FileStato;
  FILE         *ptrfile;
  char         NomeFile[300];
  int          IndicePunto;
  int          err, result;


  //----------------------------------------
  // Creazione di un file modalità INI
  //----------------------------------------
  sprintf(NomeFile, "AllenB.ini" );
  ptrfile = fopen( NomeFile, "wb");
  //---------------------------------------------------------------
  // Salvattaggio Dati
  //---------------------------------------------------------------
  if(ptrfile!=0){
     //---------------------------------------------------------------
     // Salvattaggio Sessione GENERALI
     //---------------------------------------------------------------
     sprintf(stringa,  "[GENERALI]");  Write_Line(ptrfile, stringa);    // scrittura del file di prova
     sprintf(stringa,  "PathGateway=%s",  PathGateway.c_str());     Write_Line(ptrfile, stringa);
     sprintf(stringa,  "TipoComunicazione=%d",  DatiPLC[0].TipoComunicazione);  Write_Line(ptrfile, stringa);
     sprintf(stringa,  "// 1=OPC Massimo"); Write_Line(ptrfile, stringa);
     sprintf(stringa,  "// 2=OPC Lucio"); Write_Line(ptrfile, stringa);
     sprintf(stringa,  "// 3=TCP Diretto"); Write_Line(ptrfile, stringa);
     sprintf(stringa,  "AllenbPlcIni=%d",  AllenbPlcIni);                        Write_Line(ptrfile, stringa);
     sprintf(stringa,  "AllenbTotPlc=%d",  AllenbTotPlc);                        Write_Line(ptrfile, stringa);
     sprintf(stringa,  "" );    Write_Line(ptrfile, stringa);


     //-----------------------------------------------------
     // PLC
     //-----------------------------------------------------
     for( i=1; i<=MAX_PLC_AB; i++ ){
        sprintf( stringa, "[PLC%02d]", i );
        Write_Line(ptrfile, stringa);
        sprintf(stringa,  "Topic  =%s",  DatiPLC[i].Topic );        Write_Line(ptrfile, stringa);
        sprintf(stringa,  "IpAddr =%s",  DatiPLC[i].IpAddr );       Write_Line(ptrfile, stringa);
        sprintf(stringa,  "Port   =%d",  DatiPLC[i].Port );         Write_Line(ptrfile, stringa);
        sprintf(stringa,  "" );    Write_Line(ptrfile, stringa);

        sprintf(stringa,  "RItem   =%s",  DatiPLC[i].R.Item );        Write_Line(ptrfile, stringa);
        sprintf(stringa,  "RDatoIni=%d",  DatiPLC[i].R.DatoIni );     Write_Line(ptrfile, stringa);
        sprintf(stringa,  "RNumDati=%d",  DatiPLC[i].R.NumDati );     Write_Line(ptrfile, stringa);

        sprintf(stringa,  "" );    Write_Line(ptrfile, stringa);

        sprintf(stringa,  "WItem   =%s",  DatiPLC[i].W.Item );        Write_Line(ptrfile, stringa);
        sprintf(stringa,  "WDatoIni=%d",  DatiPLC[i].W.DatoIni );     Write_Line(ptrfile, stringa);
        sprintf(stringa,  "WNumDati=%d",  DatiPLC[i].W.NumDati );     Write_Line(ptrfile, stringa);
        sprintf(stringa,  "" );    Write_Line(ptrfile, stringa);
     }
  }
  //---------------------------------------------------------------
  // Chiusura del File
  //---------------------------------------------------------------
  fclose(ptrfile);
}
//---------------------------------------------------------------------------




int  AllenB_LetturaPlc( char* all_mess )
{
  int err =0;

  //----------------------------------------
  // Interfacciamento con Massimo
  //----------------------------------------
  if( DatiPLC[ 0 ].TipoComunicazione==1 ){
     err=AllenB_MAS_DataRead_LetturaFile( all_mess );
  }
  //----------------------------------------
  // Interfacciamento con Luciano
  //----------------------------------------
  if( DatiPLC[ 0 ].TipoComunicazione==2 ){
     if( err==0 ) err = AllenB_LUC_DataRead_LetturaFile( all_mess );
  }

  //----------------------------------------
  // Interfacciamento con SLC500 TCP
  //----------------------------------------
  if( DatiPLC[ 0 ].TipoComunicazione==3 ){
     //err = AllenB_TcpSLC_DataRead( all_mess ); // Ricordarsi di inizializzare ....
     err = 1;
  }

  return err;
}
//---------------------------------------------------------------------------

int  AllenB_SctritturaPlc( char* all_mess )
{
  int err =1;
  int Plc;

  //--------------------------------------------------
  // - Inizializza Dati
  //--------------------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].W.DatiCambiati=false;
     //----------------------------
     // Verifica se dati cambiati
     //----------------------------
     if(memcmp( DatiPLC[Plc].W.Data, DatiPLC[Plc].W.OldData, sizeof(DatiPLC[Plc].W.Data))!=0){
        DatiPLC[Plc].W.DatiCambiati=true;
     }
     memcpy( DatiPLC[Plc].W.OldData, DatiPLC[Plc].W.Data, sizeof(DatiPLC[Plc].W.Data));
  }
  //----------------------------------------
  // Interfacciamento con Massimo
  //----------------------------------------
  if( DatiPLC[ 0 ].TipoComunicazione==1 ){
     err = AllenB_MAS_DataWrite_ScritturaFile( all_mess );
  }
  //----------------------------------------
  // Interfacciamento con Luciano
  //----------------------------------------
  if( DatiPLC[ 0 ].TipoComunicazione==2 ){
     err = AllenB_LUC_DataWrite_ScritturaFile( all_mess );
  }

  //----------------------------------------
  // Interfacciamento con SLC500 TCP
  //----------------------------------------
  if( DatiPLC[ 0 ].TipoComunicazione==3 ){
     //err = AllenB_TcpSLC_DataWrite( all_mess ); // Ricordarsi di inizializzare ....
     err = 1;
  }

  return err;
}
//---------------------------------------------------------------------------


// ----------------------------------------
//    AllenB_FileDoc_Rx()
// ----------------------------------------
//
//
int AllenB_FileDoc_Rx( char* NomeFile )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  //char NomeFile[300];
  char Buffer[500];
  char Buffer1[400];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;

  //strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  //sprintf( NomeFile, "DatiPlc");
  //sprintf( FileTemp, "%s.tmp"  ,  NomeFile );
  //sprintf( FileDest, "%s.txt"  ,  FileDest );
  strcpy( FileDest, NomeFile);

  ptrfile = fopen(FileDest, "wb+");
  if(ptrfile==0){
     //sprintf( all_mess, "Not open file: %s", FileDest);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     //if( DatiPLC[Plc].R.DatiOk==0 ) continue;
     //if( DatiPLC[ Plc ].DisableComunicazione ) continue;
     sprintf( Buffer, "" );
     if( DatiPLC[Plc].R.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     if( ItemTipoSlc500 ){
        sprintf( Buffer, "%02d,%s%s:%d,%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni, DatiPLC[Plc].R.NumDati    );
     }else{
        sprintf( Buffer, "%02d,%s%s[%d],%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni, DatiPLC[Plc].R.NumDati    );
     }
     //--------------------------------
     // Dati OK
     //--------------------------------
     if( DatiPLC[Plc].R.DatiOk ){
        for( i=0; i<TOT_READ_WORD; i++ ){
           if( i>=DatiPLC[Plc].R.NumDati )   break;
           //if( i>=10 )   break;
           //VarConSegno = DatiPLC[Plc].R.Data[ i ];
           //sprintf( Buffer1, ",%d", VarConSegno );
           sprintf( Buffer1, ",%04X", DatiPLC[Plc].R.Data[ i ] );
           strcat( Buffer, Buffer1 );
        }
     }else{
        strcat( Buffer, "Errore" );
     }
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  return err;
}
//---------------------------


// ----------------------------------------
//    AllenB_FileDoc_Tx()
// ----------------------------------------
//
//
int AllenB_FileDoc_Tx( char* NomeFile )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  //char NomeFile[300];
  char Buffer[500];
  char Buffer1[400];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;

  //strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  //sprintf( NomeFile, "DatiPlc");
  //sprintf( FileTemp, "%s.tmp"  ,  NomeFile );
  //sprintf( FileDest, "%s.txt"  ,  FileDest );
  strcpy( FileDest, NomeFile);

  ptrfile = fopen(FileDest, "wb+");
  if(ptrfile==0){
     //sprintf( all_mess, "Not open file: %s", FileDest);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     //if( DatiPLC[Plc].R.DatiOk==0 ) continue;
     //if( DatiPLC[ Plc ].DisableComunicazione ) continue;
     sprintf( Buffer, "" );
     if( DatiPLC[Plc].W.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     if( ItemTipoSlc500 ){
        sprintf( Buffer, "%02d,%s%s:%d,%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.DatoIni, DatiPLC[Plc].W.NumDati    );
     }else{
        sprintf( Buffer, "%02d,%s%s[%d],%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.DatoIni, DatiPLC[Plc].W.NumDati    );
     }
     //--------------------------------
     // Dati OK
     //--------------------------------
     //if( DatiPLC[Plc].R.DatiOk ){
        for( i=0; i<TOT_WRITE_WORD; i++ ){
           if( i>=DatiPLC[Plc].W.NumDati )   break;
           //if( i>=10 )   break;
           //VarConSegno = DatiPLC[Plc].R.Data[ i ];
           //sprintf( Buffer1, ",%d", VarConSegno );
           sprintf( Buffer1, ",%04X", DatiPLC[Plc].W.Data[ i ] );
           strcat( Buffer, Buffer1 );
        }
     //}else{
     //   strcat( Buffer, "Errore" );
     //}
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  return err;
}
//---------------------------



//-------------------------------------------  o  -----------------------------------
//----------------------------------------------------------------------------
// Funzioni per interfacciamento con Massimo
//----------------------------------------------------------------------------
// ----------------------------------------
//    AllenB_MAS_InitRead_ScritturaFile()
// ----------------------------------------
//
//
int AllenB_MAS_InitRead_ScritturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "InitRead");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     sprintf( Buffer, "" );
     if( DatiPLC[Plc].R.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     if( ItemTipoSlc500 ){
        sprintf( Buffer, "%02d,%s%s:%d,%d,%d",   Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni, DatiPLC[Plc].R.NumDati, DatiPLC[Plc].R.DatoIni   );
     }else{
        sprintf( Buffer, "%02d,%s%s[%d],%d,%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni, DatiPLC[Plc].R.NumDati, DatiPLC[Plc].R.DatoIni    );
     }
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  return err;
}



// ----------------------------------------
//    AllenB_MAS_InitWrite_ScritturaFile()
// ----------------------------------------
//
//
int AllenB_MAS_InitWrite_ScritturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "InitWrite");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     sprintf( Buffer, "" );
     if( DatiPLC[Plc].W.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     if( ItemTipoSlc500 ){
        // L'item non viene compilato completamente "[PLC]pippo[0]" ma "[PLC]pippo"
        // la parte "[x]" o ":x viene aggiunta dalla applicazione di massimo percè deve inserire gli item singolarmente
        sprintf( Buffer, "%02d,%s%s,%d,%d",   Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.NumDati, DatiPLC[Plc].W.DatoIni   );
     }else{
        sprintf( Buffer, "%02d,%s%s,%d,%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item,  DatiPLC[Plc].W.NumDati, DatiPLC[Plc].W.DatoIni    );
     }
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  return err;
}







// -------------------------------------
//    AllenB_MAS_DataRead_LetturaFile()
// -------------------------------------
//
//
int AllenB_MAS_DataRead_LetturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  int err1=0;
  short int VarConSegno;
  char StringaRecord[ MAX_LEN_RECORD +10 ];
  char CarattereSeparatore='_';
  int  CtrCampi=1;
  int  TotCampiReq;
  int  TotCampiRead;
  int  CampoInt;

  strcpy( all_mess, "" );
  //--------------------------------------------------
  // - Cancella l'eventuale file .TMP
  // - Rinomina File.Txt --> File Temp se possibile
  //--------------------------------------------------
  sprintf( NomeFile, "r_file");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );

  err = rename( FileDest, FileTemp );
  if( err ) sprintf( all_mess, "Impossible rename file: %s.txt to file %s.temp ", NomeFile, NomeFile);

  //--------------------------------------------------
  // - Inizializza Dati
  //--------------------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].Indice = Plc;
     DatiPLC[Plc].R.DatiOk =0;
  }

  //--------------------------------------------------
  // - Cancella l'eventuale file .TMP
  // - Rinomina File.Txt --> File Temp se possibile
  //--------------------------------------------------
  ptrfile = fopen(FileTemp, "rb");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     err=1;
  }


  //-------------------------------------
  // Recupera da File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].Indice = Plc;
     DatiPLC[Plc].R.DatiOk =0;
     DatiPLC[Plc].R.NumDatiLetti=0;
     if( err==0 ){
        err1 = !Read_Line( ptrfile, StringaRecord, MAX_LEN_RECORD);
        if( err1 ) sprintf( all_mess, "Error reading Record file %s", FileTemp );
        if( err1==0 ){
           TotCampiReq=DatiPLC[Plc].R.NumDati;
           TotCampiRead=0;
           //-------------------------------
           // Cerca il campo nel Record
           //-------------------------------
           AllenB_GetTuttiCampiRecord( TotCampiReq, &TotCampiRead, StringaRecord, CarattereSeparatore, all_mess );
           DatiPLC[Plc].R.NumDatiLetti=TotCampiRead;
           //----------------------------
           // Estrai Dato
           //----------------------------
           if( TotCampiRead>=TotCampiReq ){
              DatiPLC[Plc].R.DatiOk =true;
              //----------------------------
              // Estrai Dato
              //----------------------------
              for( CtrCampi=1; CtrCampi<=TotCampiRead; CtrCampi++ ){
                 if( CtrCampi>=MAX_WORD_PLC ) break;
                 CampoInt = atoi( &PtrStrCampo[ CtrCampi ][ 0 ] );
                 DatiPLC[Plc].R.Data[ CtrCampi-1 ] = CampoInt;
              }
           }
        }
     }
     //--------------------------
     // Ctr Errori Consecutivi
     //--------------------------
     if( err || err1 ){
        if( DatiPLC[Plc].CtrErroriConsecutivi<10 ) DatiPLC[Plc].CtrErroriConsecutivi++;
        if( DatiPLC[Plc].CtrErroriConsecutivi> 9 ) DatiPLC[Plc].ErrorePersistente=true;
        if( DatiPLC[Plc].DisableComunicazione    ) DatiPLC[Plc].ErrorePersistente=false;
        continue;
     }else{
        DatiPLC[Plc].CtrErroriConsecutivi = 0;
        DatiPLC[Plc].ErrorePersistente    =false;
     }
  }
  fclose(ptrfile);

  return err;
}



// --------------------------------------------
//    AllenB_MAS_DataWrite_ScritturaFile()
// --------------------------------------------
//
//
int AllenB_MAS_DataWrite_ScritturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "w_file");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     sprintf( Buffer, "" );
     for( i=0; i<TOT_WRITE_WORD; i++ ){
        if( i>=DatiPLC[Plc].W.NumDati ){
            break;
        }
        VarConSegno = DatiPLC[Plc].W.Data[ i ];
        if( i==0 ) sprintf( Buffer1,  "%d", VarConSegno );
        if( i >0 ) sprintf( Buffer1, "_%d", VarConSegno );
        strcat( Buffer, Buffer1 );
     }
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  return err;
}


//--------------------------------  o  ----------------------------------

//----------------------------------------------------------------------------
// Funzioni per interfacciamento con LUCIANO
//----------------------------------------------------------------------------

// ----------------------------------------
//    AllenB_LUC_CompilazioneFileTagCFG()
// ----------------------------------------
int AllenB_LUC_CompilazioneFileTagCFG( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;
  int TotTagsRead;
  int TotTagsWrite;
  int CtrTagsTot=0;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "TagCfg");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.cfg"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione Intestazione
  //-------------------------------------
  Write_Line( ptrfile, "; Dopo il # Nr DB appoggio supervisore ricordarsi di inserire");
  Write_Line( ptrfile, "; sempre 4 cifre perchè viene eseguito un get di 4 caratteri senza ");
  Write_Line( ptrfile, "; ulteriori controlli ");
  Write_Line( ptrfile, "; ");
  Write_Line( ptrfile, "; I tag devono essere inseriti prima le DB poi ingressi poi uscite e infine i merker allarmi ");
  Write_Line( ptrfile, "; ");
  Write_Line( ptrfile, ";OFF_D,32 	Nr di word DB da leggere ");
  Write_Line( ptrfile, ";OFF_X,11 	Nr di word di ingressi da laggere ");
  Write_Line( ptrfile, ";OFF_Y,9	Nr di word di uscite da leggere ");
  Write_Line( ptrfile, ";OFF_M,9	Nr di word di bit M da legger ");
  Write_Line( ptrfile, ";OFF_A,11	Nr di word di bit A da leggere ");
  Write_Line( ptrfile, ";Fare molta attenzione a inserire il Nr corretto di word con il corrispondente nr di Item ");
  Write_Line( ptrfile, ";perchè il programma usa questi numeri per calcolarsi l'offset per fare le varie ");
  Write_Line( ptrfile, ";operazioni di lettura e scrittura ");
  Write_Line( ptrfile, ";");
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, " ");


  //-------------------------------------
  // Calcolo dati dei TAGS complessivi
  //-------------------------------------
  TotTagsRead=0;
  TotTagsWrite=0;
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     if( Plc>AllenbTotPlc ) break;

     TotTagsRead = TotTagsRead  + DatiPLC[Plc].R.NumDati;
     TotTagsWrite= TotTagsWrite + DatiPLC[Plc].W.NumDati;
  }

  //-------------------------------------
  // Compilazione TAGS LETTURA
  //-------------------------------------
  Write_Line( ptrfile, ";-------- LETTURE ---------");
  Write_Line( ptrfile, " ");
  sprintf( Buffer, "OFF_D,%d", TotTagsRead ), Write_Line( ptrfile, Buffer );
  Write_Line( ptrfile, "OFF_X,0");
  Write_Line( ptrfile, "OFF_Y,0");
  Write_Line( ptrfile, "OFF_M,0");
  Write_Line( ptrfile, "OFF_A,0");
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, " ");

  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     if( Plc>AllenbTotPlc ) break;
     if( DatiPLC[Plc].R.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     for( i=0; i<TOT_READ_WORD; i++ ){
        if( i>=DatiPLC[Plc].R.NumDati ) break;
        if( ItemTipoSlc500 ){
           sprintf( Buffer, "%s%s:%d,L1 #%04d",  DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni+i, CtrTagsTot    );
        }else{
           sprintf( Buffer, "%s%s[%d],L1 #%04d",  DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni+i, CtrTagsTot    );
        }
        CtrTagsTot++;
        Write_Line( ptrfile, Buffer);
     }
  }
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, " ");


  //-------------------------------------
  // Compilazione TAGS SCRITTURA
  //-------------------------------------
  Write_Line( ptrfile, ";-------- SCRITTURE ---------");
  Write_Line( ptrfile, ";");
  Write_Line( ptrfile, ";");
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, ";OFW_D,			' Nr word DB in scrittura su DB del PLC");
  Write_Line( ptrfile, ";OFW_M,			' Nr word bit M in scrittura su bit M del PLC");
  Write_Line( ptrfile, ";OFW_A,			' Non utilizzato");
  Write_Line( ptrfile, ";OFWMD,			' Nr word bit M scritti su DB del PLC");
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, " ");

  sprintf( Buffer, "OFW_D,%d", TotTagsWrite ), Write_Line( ptrfile, Buffer );
  Write_Line( ptrfile, "OFF_X,0");
  Write_Line( ptrfile, "OFW_M,0");
  Write_Line( ptrfile, "OFW_A,0");
  Write_Line( ptrfile, "OFWMD,0");
  Write_Line( ptrfile, "OFWR1,0");
  Write_Line( ptrfile, "OFWR2,0");
  Write_Line( ptrfile, " ");
  Write_Line( ptrfile, " ");

  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
    if( Plc>AllenbTotPlc ) break;
     if( DatiPLC[Plc].W.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     for( i=0; i<TOT_READ_WORD; i++ ){
        if( i>=DatiPLC[Plc].W.NumDati ) break;
        if( ItemTipoSlc500 ){
           sprintf( Buffer, "%s%s:%d,L1 #%04d",  DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.DatoIni+i, CtrTagsTot    );
        }else{
           sprintf( Buffer, "%s%s[%d],L1 #%04d",  DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.DatoIni+i, CtrTagsTot    );
        }
        CtrTagsTot++;
        Write_Line( ptrfile, Buffer);
     }
  }
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  return err;
}



// -----------------------------------------------------------------
//    AllenB_LUC_CompilazioneFileOpcCFG()
//  Definisce quali variabili visualizzare sulla pagina principale
// -----------------------------------------------------------------
int AllenB_LUC_CompilazioneFileOpcCFG( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;
  int TotTagsRead;
  int TotTagsWrite;
//  int CtrTagsTot=0;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "Opc");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.cfg"  , PercorsoAllenB, NomeFile );
  unlink( FileTemp );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione Intestazione
  //-------------------------------------
/*
;
;ALLCO nr allarme di comunicazione
;
;DATAB,Provider=SQLOLEDB.1;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=MartelliSalumi;Data Source=SERVERFB
DATAB,Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Initial Catalog=Martelli Salumi;Data Source=(Local)
;DATAB,Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Pwd=ordinifb;Initial Catalog=MartelliSalumi;Data Source=10.0.4.1
SIMUL,0
TIMER,1
SERVE,RSLinx OPC Server
GROUP,MyOPCGroupr
*/
  Write_Line( ptrfile, ";");
  Write_Line( ptrfile, ";ALLCO nr allarme di comunicazione");
  Write_Line( ptrfile, ";");
  Write_Line( ptrfile, ";DATAB,Provider=SQLOLEDB.1;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=MartelliSalumi;Data Source=SERVERFB ");
  Write_Line( ptrfile, "DATAB,Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Initial Catalog=Martelli Salumi;Data Source=(Local) ");
  Write_Line( ptrfile, ";DATAB,Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Pwd=ordinifb;Initial Catalog=MartelliSalumi;Data Source=10.0.4.1");
  Write_Line( ptrfile, "SIMUL,0");
  Write_Line( ptrfile, "TIMER,1");
  Write_Line( ptrfile, "SERVE,RSLinx OPC Server");
  Write_Line( ptrfile, "GROUP,MyOPCGroup");
  Write_Line( ptrfile, " ");
  //------------------------------------------
  // Compilazione TAGS LETTURA PAG INIZIALE
  //------------------------------------------
  //TAGRW,0000,[AUTOCL_A]N238:196,L1
  //TAGRW,0001,[AUTOCL_B]N238:196,L1
  //TAGRW,0025,None

  //  for( Plc=1; Plc<=TOT_PLC_AB; Plc++ ){
  for( Plc=0; Plc<=33; Plc++ ){
     if( Plc>0 && Plc<=AllenbTotPlc ){
        if( DatiPLC[Plc].R.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
        if( ItemTipoSlc500 ){
           //sprintf( Buffer, "TAGRW,%04d,%s%s:%d,L1 ",  Plc, DatiPLC[Plc+1].Topic, DatiPLC[Plc+1].R.Item, DatiPLC[Plc+1].R.DatoIni    );
           sprintf( Buffer, "TAGRW,%04d,%s%s:%d,L1",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni    );
        }else{
           //sprintf( Buffer, "TAGRW,%04d,%s%s[%d],L1",  Plc, DatiPLC[Plc+1].Topic, DatiPLC[Plc+1].R.Item, DatiPLC[Plc+1].R.DatoIni    );
           sprintf( Buffer, "TAGRW,%04d,%s%s[%d],L1",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni    );
        }
     }else{
        sprintf( Buffer, "TAGRW,%04d,None",  Plc);
     }
     Write_Line( ptrfile, Buffer);
  }
  Write_Line( ptrfile, " ");

  Write_Line( ptrfile, "LOCAL,0");
  Write_Line( ptrfile, "SALVA,1 ");
  Write_Line( ptrfile, "ALLCO,400");
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  return err;
}





// ----------------------------------------
//    AllenB_LUC_InitRead_ScritturaFile()
// ----------------------------------------
//
//
int AllenB_LUC_InitRead_ScritturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;

  strcpy( all_mess, "" );
  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "InitRead");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     sprintf( Buffer, "" );
     if( DatiPLC[Plc].R.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     if( ItemTipoSlc500 ){
        sprintf( Buffer, "%02d,%s%s:%d,%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni, DatiPLC[Plc].R.NumDati    );
     }else{
        sprintf( Buffer, "%02d,%s%s[%d],%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].R.Item, DatiPLC[Plc].R.DatoIni, DatiPLC[Plc].R.NumDati    );
     }
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  return err;
}



// ----------------------------------------
//    AllenB_LUC_DataRead_LetturaFile()
// ----------------------------------------
//
//
int AllenB_LUC_DataRead_LetturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char FileSemaforoMio[300];
  char FileSemaforoSuo[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  int err1=0;
  short int VarConSegno;
  char StringaRecord[ MAX_LEN_RECORD +10 ];
  char CarattereSeparatore=',';
  int  CtrCampi=1;
  int  TotCampiReq;
  int  TotCampiRead;
  int  CampoInt;
  bool FileAperto=false;
  struct stat st;

  strcpy( all_mess, "" );
  //--------------------------------------------------
  // - Cancella l'eventuale file .TMP
  // - Rinomina File.Txt --> File Temp se possibile
  //--------------------------------------------------
  sprintf( NomeFile, "DataRead");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  sprintf( FileSemaforoSuo, "%s\\Semaforo%s.txt"  , PercorsoAllenB, NomeFile );
  sprintf( FileSemaforoMio, "%s\\Semaforo%s.txt"  , PercorsoAllenB, "InitRead" );

//  unlink( FileTemp );
//  struct stat st;
//  if (stat (FileSemaforoSuo, &st) != 0)   //    return 0; sembra + performante

  //--------------------------------------------------
  // - Inizializza Dati
  //--------------------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].Indice = Plc;
     DatiPLC[Plc].R.DatiOk =0;
     DatiPLC[Plc].R.DatiCambiati=false;
  }

  //--------------------------------------------------
  // Verifica sei dati sono pronti
  //--------------------------------------------------
  if( access( FileSemaforoSuo, 0 )!=0 ){
  //if( stat(FileSemaforoSuo, &st)!= 0) return 0;
  //if( stat(FileSemaforoSuo, &st)!= 0){   //    return 0; sembra + performante
     err=1;
     sprintf( all_mess, "Dati Non pronti");
  }else{
     unlink( FileTemp );
     err = rename( FileDest, FileTemp );
     if( err ) sprintf( all_mess, "Impossible rename file: %s.txt to file %s.temp ", NomeFile, NomeFile);
  }

  //--------------------------------------------------
  // - Cancella l'eventuale file .TMP
  // - Rinomina File.Txt --> File Temp se possibile
  //--------------------------------------------------
  if( err==0 ){
     ptrfile = fopen(FileTemp, "rb");
     if(ptrfile==0){
        sprintf( all_mess, "Not open file: %s", FileTemp);
        fclose(ptrfile);
        err=1;
     }else{
        FileAperto=true;
     }
  }
  //-------------------------------------
  // Recupera da File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=1; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].Indice = Plc;
     DatiPLC[Plc].R.DatiOk =0;
     DatiPLC[Plc].R.NumDatiLetti=0;
     if( err==0 ){
        err1 = !Read_Line( ptrfile, StringaRecord, MAX_LEN_RECORD);
        if( err1 ) sprintf( all_mess, "Error reading Record file %s", FileTemp );
        if( err1==0 ){
           TotCampiReq= DatiPLC[Plc].R.NumDati+4 ;
           TotCampiRead=0;
           //-------------------------------
           // Cerca il campo nel Record
           //-------------------------------
           AllenB_GetTuttiCampiRecord( TotCampiReq, &TotCampiRead, StringaRecord, CarattereSeparatore, all_mess );
           DatiPLC[Plc].R.NumDatiLetti=TotCampiRead;
           if( TotCampiRead< TotCampiReq ) err1=1;              // I dati ricevuti non sono corretti
           if( atoi( &PtrStrCampo[ 1 ][ 0 ]) != Plc ) err1=1;   // La riga non corrisponde al PLC
           if( atoi( &PtrStrCampo[ 4 ][ 0 ]) == 0   ) err1=1;   // Dati non letti --> errore
           //----------------------------
           // Estrai Dato
           //----------------------------
           if( err1==0 ){
              DatiPLC[Plc].R.DatiOk =true;
              memcpy( DatiPLC[Plc].R.OldData, DatiPLC[Plc].R.Data, sizeof(DatiPLC[Plc].R.Data));
              //----------------------------
              // Estrai Dato
              //----------------------------
              for( CtrCampi=5; CtrCampi<=TotCampiRead; CtrCampi++ ){
                 if( CtrCampi>=MAX_WORD_PLC ) break;
                 CampoInt = atoi( &PtrStrCampo[ CtrCampi ][ 0 ] );
                 DatiPLC[Plc].R.Data[ CtrCampi-5 ] = CampoInt;
              }
              //----------------------------
              // Verifica se dati cambiati
              //----------------------------
              if(memcmp( DatiPLC[Plc].R.Data, DatiPLC[Plc].R.OldData, sizeof(DatiPLC[Plc].R.Data))!=0){
                 DatiPLC[Plc].R.DatiCambiati=true;
              }
           }
        }
     }
     //--------------------------
     // Ctr Errori Consecutivi
     //--------------------------
     if( err || err1 ){
        if( DatiPLC[Plc].CtrErroriConsecutivi<10 ) DatiPLC[Plc].CtrErroriConsecutivi++;
        if( DatiPLC[Plc].CtrErroriConsecutivi> 9 ) DatiPLC[Plc].ErrorePersistente=true;
        if( DatiPLC[ Plc ].DisableComunicazione  ) DatiPLC[Plc].ErrorePersistente=false;
        continue;
     }else{
        DatiPLC[Plc].CtrErroriConsecutivi   =0;
        DatiPLC[Plc].ErrorePersistente      =false;
     }
  }
  if( FileAperto )  fclose(ptrfile);

  //--------------------------------------
  // Cancella semaforo Suo
  // Crea Semaforo Mio
  //--------------------------------------
  unlink( FileSemaforoSuo );

  if( access( FileSemaforoMio, 0 )!=0 ){
     ptrfile = fopen(FileSemaforoMio, "wb+");
     fclose( ptrfile );
  }

  return err;
}



// ----------------------------------------
//    AllenB_LUC_DataWrite_ScritturaFile()
// ----------------------------------------
//
//
int AllenB_LUC_DataWrite_ScritturaFile( char* all_mess )
// *********************************
{
  char FileTemp[300];
  char FileDest[300];
  char NomeFile[300];
  char Buffer[500];
  char Buffer1[100];
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  short int VarConSegno;
  bool ItemTipoSlc500;
  char FileSemaforoMio[300];
  strcpy( all_mess, "" );


  //-------------------------------------------------
  // Scrivi il file .TMP
  //-------------------------------------------------
  sprintf( NomeFile, "DataWrite");
  sprintf( FileTemp, "%s\\%s.tmp"  , PercorsoAllenB, NomeFile );
  sprintf( FileDest, "%s\\%s.txt"  , PercorsoAllenB, NomeFile );
  sprintf( FileSemaforoMio, "%s\\Semaforo%s.txt"  , PercorsoAllenB, NomeFile );
  unlink( FileDest );

  ptrfile = fopen(FileTemp, "wb+");
  if(ptrfile==0){
     sprintf( all_mess, "Not open file: %s", FileTemp);
     fclose(ptrfile);
     return 1;
  }

  //-------------------------------------
  // Compilazione File di TESTO
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     if( DatiPLC[Plc].R.DatiOk==0 ) continue;
     if( DatiPLC[ Plc ].DisableComunicazione ) continue;
     sprintf( Buffer, "" );
     if( DatiPLC[Plc].W.Item[0] == 'N' ) ItemTipoSlc500=true; else ItemTipoSlc500=false;
     if( ItemTipoSlc500 ){
        sprintf( Buffer, "%02d,%s%s:%d,%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.DatoIni, DatiPLC[Plc].W.NumDati    );
     }else{
        sprintf( Buffer, "%02d,%s%s[%d],%d",  Plc, DatiPLC[Plc].Topic, DatiPLC[Plc].W.Item, DatiPLC[Plc].W.DatoIni, DatiPLC[Plc].W.NumDati    );
     }
     for( i=0; i<TOT_WRITE_WORD; i++ ){
        if( i>=DatiPLC[Plc].W.NumDati )   break;
        VarConSegno = DatiPLC[Plc].W.Data[ i ];
        sprintf( Buffer1, ",%d", VarConSegno );
        strcat( Buffer, Buffer1 );
     }
     Write_Line( ptrfile, Buffer);
  }
  fclose(ptrfile);

  //-----------------------------------------
  // Rinomina file --> FileDest
  // La cosa avrà successo solo se FileDest
  // è già stato cancellato
  //-----------------------------------------
  err = rename(FileTemp, FileDest);
  if( err ) sprintf( all_mess, "Impossible rename file: %s.tmp to file %s.txt ", NomeFile, NomeFile);

  //Accendi semaforo cmpilazione completata
  if( access( FileSemaforoMio, 0 )!=0 ){
     ptrfile = fopen(FileSemaforoMio, "wb+");
     fclose( ptrfile );
  }

  return err;
}
//---------------------------



//--------------------------------  o  ----------------------------------

//----------------------------------------------------------------------------
// Funzioni per interfacciamento con SLC500 via TCP
//----------------------------------------------------------------------------


// ----------------------------------------
//    AllenB_TcpSLC_DataRead()
// ----------------------------------------
//
//
int AllenB_TcpSLC_DataRead( char* all_mess )
// *********************************
{
  FILE *ptrfile;
  int Plc,i;
  int err=0;
  int err1=0;

  strcpy( all_mess, "" );
  //--------------------------------------------------
  // - Inizializza Dati
  //--------------------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].Indice = Plc;
     DatiPLC[Plc].R.DatiOk =0;
     DatiPLC[Plc].R.DatiCambiati=false;
  }
  //-----------------------------
  // Multi PLC
  //-----------------------------
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     //SlcPlc[Plc].Multi_NumFile=7;
     //SlcPlc[Plc].Multi_Data_Ini=10*(Plc-1);
     //SlcPlc[Plc].Multi_Data_Len=10;
     SlcPlc[Plc].Multi_NumFile= atoi( &DatiPLC[Plc].R.Item[1] ) ;
     SlcPlc[Plc].Multi_Data_Ini=DatiPLC[Plc].R.DatoIni;
     SlcPlc[Plc].Multi_Data_Len=DatiPLC[Plc].R.NumDati;
  }
  Slc500_PcccMultiLeggiWord ( all_mess );

  //-------------------------------------
  // Recupera da Dati
  // Indice, Aux, NumDati, Dat1, Dat2...
  //-------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     err1=1;
     DatiPLC[Plc].Indice = Plc;
     DatiPLC[Plc].R.DatiOk =0;
     DatiPLC[Plc].R.NumDatiLetti=0;
     if( SlcPlc[Plc].ErrPlc==0 && SlcPlc[Plc].MultiRxPlcOk ){
        DatiPLC[Plc].R.DatiOk =true;
        DatiPLC[Plc].R.NumDatiLetti=DatiPLC[Plc].R.NumDati;
        memcpy( DatiPLC[Plc].R.OldData, DatiPLC[Plc].R.Data, sizeof(DatiPLC[Plc].R.Data));
        memcpy( DatiPLC[Plc].R.Data, SlcPlc[Plc].Multi_W_TxRx, DatiPLC[Plc].R.NumDati*2 );
        //----------------------------
        // Verifica se dati cambiati
        //----------------------------
        if(memcmp( DatiPLC[Plc].R.Data, DatiPLC[Plc].R.OldData, sizeof(DatiPLC[Plc].R.Data))!=0){
           DatiPLC[Plc].R.DatiCambiati=true;
        }
        err1=0;
     }
     //--------------------------
     // Ctr Errori Consecutivi
     //--------------------------
     if( err1 ){
        if( DatiPLC[Plc].CtrErroriConsecutivi<10 ) DatiPLC[Plc].CtrErroriConsecutivi++;
        if( DatiPLC[Plc].CtrErroriConsecutivi> 9 ) DatiPLC[Plc].ErrorePersistente=true;
        if( DatiPLC[ Plc ].DisableComunicazione  ) DatiPLC[Plc].ErrorePersistente=false;
        continue;
     }else{
        DatiPLC[Plc].CtrErroriConsecutivi   =0;
        DatiPLC[Plc].ErrorePersistente      =false;
     }
  }
  SlcPlc[25].Multi_W_TxRx[0] = SlcPlc[25].Multi_W_TxRx[0];

  return err;
}




// ----------------------------------------
//    AllenB_TcpSLC_DataWrite()
// ----------------------------------------
//
//
int AllenB_TcpSLC_DataWrite( char* all_mess )
// *********************************
{
  FILE *ptrfile;
  int Plc,i;
  int err=0;
//  int err1=0;

  strcpy( all_mess, "" );
  //--------------------------------------------------
  // - Inizializza Dati
  //--------------------------------------------------
  for( Plc=AllenbPlcIni; Plc<=AllenbTotPlc; Plc++ ){
     DatiPLC[Plc].Indice = Plc;
     //DatiPLC[Plc].R.DatiOk =0;
  }
  //-----------------------------
  // Compila Dati PLC
  //-----------------------------
  for( Plc=1; Plc<=Slc500_TotPlc; Plc++){
     //SlcPlc[Plc].Multi_NumFile=7;
     //SlcPlc[Plc].Multi_Data_Ini=10*(Plc-1);
     //SlcPlc[Plc].Multi_Data_Len=10;
     SlcPlc[Plc].Multi_NumFile= atoi( &DatiPLC[Plc].W.Item[1] ) ;
     SlcPlc[Plc].Multi_Data_Ini=DatiPLC[Plc].W.DatoIni;
     SlcPlc[Plc].Multi_Data_Len=DatiPLC[Plc].W.NumDati;
     memcpy( SlcPlc[Plc].Multi_W_TxRx, DatiPLC[Plc].W.Data, DatiPLC[Plc].W.NumDati*2 );
  }

  err = Slc500_PcccMultiScriviWord ( all_mess );
  return err;
}

