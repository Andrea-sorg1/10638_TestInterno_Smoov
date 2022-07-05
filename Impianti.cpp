//-----------------------------------------------------------------
//                  file     IMPIANTI.CPP
//-----------------------------------------------------------------
//
// Questo file contiene tutte le funzioni specialòi create appositamente
// per i vari Impianti.
//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>
#include <stdio.h>
#include <share.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <dos.h>
#include <errno.h>
#include <vcl\inifiles.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>

#pragma hdrstop

#include "Main.h"
#include "Tecnofer.h"
#include "Remoto.h"
#include "Impianti.h"
#include "Libreria.h"
#include "ComunicaSiemens.h"


//---------------------------------------------------------------------------
// PLUGING IMPIANTI
//---------------------------------------------------------------------------
#include PLANT_PLUGIN

//---------------------------------------------------------------------------
#pragma package(smart_init)
//
//---------------------------------------------------------------------------
//-------------------------------
//   disegna_speciali()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN LAYOUT.CPP  (AggiornaSinottico())
//---------------------------------------------------------------------------
// Rilettura flag riservati all'impianto
//
//
void disegna_speciali_IMPIANTI()
{
  disegna_speciali_ICAM();
  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   ReadIni_SPEC()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN MAIN.CPP  (ReadIniProject())
//---------------------------------------------------------------------------
// Rilettura flag riservati all'impianto
//
//
void ReadIni_IMPIANTI()
{
  ReadIni_ICAM();
  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   WriteIni_SPEC()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN MAIN.CPP  (Close())
//---------------------------------------------------------------------------
// Scrittura flag riservati all'impianto
//
void WriteIni_IMPIANTI()
{
  WriteIni_ICAM();
  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   VerificaInputMAG_IMPIANTI()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN MAGWIN  (Salva())
//---------------------------------------------------------------------------
// Forzature editor magazzini legate ai singoli impianti ALL_GLASS
//
void VerificaInputMAG_IMPIANTI(struct MAG *mag)
{
  VerificaInputMAG_ICAM(mag);
  return;
}
//---------------------------------------------------------------------------
//-------------------------------
//   VerificaInputMAG_IMPIANTI()
//-------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN BOXWIN  (Salva())
//---------------------------------------------------------------------------
// Forzature editor magazzini legate ai singoli impianti ALL_GLASS
//
void VerificaInputBOX_IMPIANTI(struct BOX *box)
{
  VerificaInputBOX_ICAM(box);
  return;
}
//---------------------------------------------------------------------------
//---------------------------------
// AdjustDMStartMission_IMPIANTO()
//---------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN AGV.CPP  (trs_mission())
//---------------------------------------------------------------------------
// Aggiusta il DM di start missione (D3000...D3060) con gestioni specializzate
// per singolo impianto
//
void AdjustDMStartMission_IMPIANTO(short int num_agv, WORD *dm)
{
  AdjustDMStartMission_ICAM(num_agv, dm);
  return;
}
//---------------------------------------------------------------------------
// -----------------------------------
//  load_chiamate_baie_IMPIANTI()
// -----------------------------------
//  Personalizzazione per l'impianto OI-MEZZACORONA (ALLGLASS)
//---------------------------------------------------------------------------
// UTILIZZATA IN PROCESS.CPP  (load_chiamate_baie())
//---------------------------------------------------------------------------
// E' una funzione che legge i segnali e i dati da PLC SIEMENS S300 (ETH)
// con le DB di scambio defite per l'impianto MEZZACORONA
//
int load_chiamate_baie_IMPIANTI(char *all_mess)
/********************************************/
{
  int err=0;

  err = load_chiamate_baie_ICAM(all_mess);

  return err;
}
//---------------------------------------------------------------------------
// -----------------------------------
//  write_chiamate_baie_IMPIANTI()
// -----------------------------------
//  Personalizzazione per l'impianto OI-MEZZACORONA (ALLGLASS)
//---------------------------------------------------------------------------
// UTILIZZATA IN PROCESS.CPP  (write_chiamate_baie())
//---------------------------------------------------------------------------
// E' una funzione che scrive i segnali e i dati da PLC SIEMENS S300 (ETH)
// con le DB di scambio defite per l'impianto MEZZACORONA
//
int write_chiamate_baie_IMPIANTI(char *all_mess)
//*****************************************
{
  int err=0;

  err = write_chiamate_baie_ICAM(all_mess);

  return err;
}
//---------------------------------------------------------------------------

// --------------------------------------------
//  SetResetPIntForzatoTGV_IMPIANTI()
// --------------------------------------------
//  Personalizzazione per l'impianto
//-----------------------------------------------------------------------i----
// UTILIZZATA IN AGV.CPP  (SetResetPIntForzatoTGV_IMPIANTI())
//
//---------------------------------------------------------------------------
// Rielabora i flags di SetReset PIntForzato
//
void SetResetPIntForzatoTGV_IMPIANTI(short int num_agv, bool *ResetMissione, bool *ResetChiamata, bool *ResetPIntForzato, short int *ForzaPInt)
{
  SetResetPIntForzatoTGV_ICAM(num_agv, ResetMissione, ResetChiamata, ResetPIntForzato, ForzaPInt);
}
//---------------------------------------------------------------------------
// --------------------------------------------
//  AvvicinamentoADestinazione_IMPIANTI()
// --------------------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN AGV.CPP  (AvvicinamentoADestinazione_IMPIANTI())
//
//---------------------------------------------------------------------------
// Rielabora i flags di SetReset PIntForzato
//
void AvvicinamentoADestinazione_IMPIANTI(short int dest, short int *perc, struct DATI_PERCORSO *dati, short int *pint)
{
  AvvicinamentoADestinazione_ICAM(dest, perc, dati, pint);
}
//---------------------------------------------------------------------------
// --------------------------------------------
//  PosizioneRiposo_IMPIANTI()
// --------------------------------------------
//  Personalizzazione per l'impianto
//---------------------------------------------------------------------------
// UTILIZZATA IN PRCHIAMA.CPP  (PosizioneRiposoTGV())
//
//---------------------------------------------------------------------------
// Ritorna 1 se il posizionamento a RIPOSO deve essere lasciato alla funzione standard
//
int PosizioneRiposo_IMPIANTI(short int num_agv, char *all_mess)
{
  int typ_riposo;

  //-------------------------------------------------
  //
  //-------------------------------------------------
  //-------------------------------------------------
  // Ritorna "1" se il processo di ritorno a riposo
  // deve essere lasciato alla funzione STANDARD
  //-------------------------------------------------
  typ_riposo = PosizioneRiposo_ICAM(num_agv, all_mess);

  return typ_riposo;
}
//---------------------------------------------------------------------------

