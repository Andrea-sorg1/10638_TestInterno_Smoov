//---------------------------------------------------------------------------
#include <vcl.h>
#include <time.h>
#pragma hdrstop

#include <stdlib.h>
#include <sys\timeb.h>
#include "Thread1.h"
#include "Main.h"
#include "SQLDB.h"
#include "TecnoFer.h"

#pragma package(smart_init)

// Infos relatives au threads
//---------------------------------------------------------------------------
//   Important : les méthodes et les propriétés des objets dans la VCL ne peuvent
//   être utilisées que dans une méthode appelée en utilisant Synchronize, par exemple :
//
//      Synchronize(UpdateCaption);
//
//   où UpdateCaption pourrait être du type :
//
//      void __fastcall TMyThread::UpdateCaption()
//      {
//        Form1->Caption = "Mise à jour dans un thread";
//      }
//---------------------------------------------------------------------------
__fastcall TMyThread1::TMyThread1()
    : TThread(false)
{
  CountThread     = 0;
  TimeLastThread  = 0;
  FreeOnTerminate = true;
  Priority        = tpLower;
}
//---------------------------------------------------------------------------
void __fastcall TMyThread1::Execute()
{
  long int DiffTime;
  long int TimeAttuale;

  while(!Terminated)
  {
     TimeAttuale = time(NULL);
     DiffTime    = TimeAttuale - TimeLastThread;
     if(DiffTime>SecReloadOnThreadSQL){
        //----------------------------------------------------------------
        // Contatore di richiamo del thread
        //----------------------------------------------------------------
        CountThread++;
        if(CountThread>99) CountThread=1;
        //----------------------------------------------------------------
        // - Richiamo di una funzione sincronizzata con gli altri Thread
        // - Invio di messaggi all'applicazione principale
        //----------------------------------------------------------------
        Synchronize(FunzioneSincrona);
        Application->ProcessMessages();
        TimeLastThread = TimeAttuale;
     }
  }
}
//---------------------------------------------------------------------------
//-----------------------------------------------------------------------
// Funzione che viene richiamata in maniera sincronizzata con gli altri
// Thread dell'applicazione
//-----------------------------------------------------------------------
void __fastcall TMyThread1::FunzioneSincrona()
{
  int  msec, err;
  char all_mess[101]="";
  SYSTEMTIME TimeLoadSQLStart;
  SYSTEMTIME TimeLoadSQLEnd;

  //-------------------------------------------------------
  // Esecuzione comunicazione SQL
  //-------------------------------------------------------
  GetLocalTime( &TimeLoadSQLStart );        // start lettura da SQL SERVER
  Application->ProcessMessages();
  err = FormSQLDB->load_udc_from_SQL(CMD_RD_SQL_ALL_CHANGE, 0, 0, "", "", all_mess);
  if(err) P.error_trasmit(0, all_mess);
  GetLocalTime( &TimeLoadSQLEnd );         // start lettura da SQL SERVER
  //-------------------------------------------------------
  // Visualizzazione tempi SQL
  //-------------------------------------------------------
  msec = (int) (TimeLoadSQLEnd.wSecond - TimeLoadSQLStart.wSecond);
  msec = msec * 1000;
  msec = msec + (int) (TimeLoadSQLEnd.wMilliseconds - TimeLoadSQLStart.wMilliseconds);
  if( msec<0 ) msec=0;
  sprintf(all_mess, "(th%02d) %dmsec", CountThread, msec);
  FormMain->SBarImpianto->Panels->Items[7]->Text = all_mess;
}
//---------------------------------------------------------------------------
