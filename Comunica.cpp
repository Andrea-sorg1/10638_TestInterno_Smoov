// ------------------------------------------------------------------------
//                     file     COMUNICA.CPP
// ------------------------------------------------------------------------
#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include "Main.h"
#include "AlgoScambio.h"
#include "tecnofer.h"
#include "variabil.h"
#include "OmronETH.h"
#include "OmronEthSer.h"
#include "OmronEthTcp.h"
#include "Modbus.h"

WORD AttesaRisposta=1;

// --------------------------------
//      init_porta()
// --------------------------------
// inizializzazione porta seriale
//
WORD omron::init_porta(WORD port, int TimeOut, char *all_mess)
/*********************************************/
{
  WORD err=0;
  int  Baudrate;
  WORD Dati;
  char Parita;
  WORD StopBit;

  //------------------------------------------------------------
  // Recupero delle informazioni dalla struttura
  //------------------------------------------------------------
  Baudrate = InCom[port].Baudrate;
  Dati     = InCom[port].Dati;
  Parita   = InCom[port].Parita;
  StopBit  = InCom[port].StopBit;

  //err = WINCOM[port].init_port(port, Baudrate, Dati, Parita, StopBit, TimeOut);
  //if(!err) err=WINCOM[port].setup_timeout( port, 20, TimeOut, 100, TimeOut, 100);
  err = init_port(port, Baudrate, Dati, Parita, StopBit, TimeOut);
    if(err) {
     sprintf(all_mess, "Error detected setting COM %d", port+1);
     return 1;
  }
  return 0;
}


// -----------------------------------
//      md()
// -----------------------------------
// imposta il modo di funzionamento del PLC:    0 - program
//                                              2 - monitor
//                                              3 - run  ( default )
WORD omron::md(WORD plc, WORD md, char *all_mess)
/**********************************************************/
{
  WORD err=0;
  WORD Port;
  WORD IdPlc;
  int  Tipo;

  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
  IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Comunicazione HOST LINK
  //---------------------------------------------
  if(Tipo==0){
     err = modo(Port, IdPlc, md, all_mess);
  }
  return err;
}

// ------------------------------------------
//      ld_canale()
// ------------------------------------------
// Legge una sequenza di canali per un massimo di 25 canali. In caso di errore
// ripete la trasmissione e restituisce l'errore dopo fact_err ripetizioni.
//
WORD omron::ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  WORD err=0;
  WORD rip1;
  WORD TipoComando;
  WORD TipoVariabile;
  WORD AttesaRisposta;
  WORD Port;
  WORD IdPlc;
  int  Tipo;

  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
  IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Selezione porta di comunicazione
  //---------------------------------------------
  Port=COM1;                             // TGVs
  if(plc==0               ) Port=COM2;   // PLC TERRA
  if(plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //---------------------------------------------
  // Verifico se la comunicazione è disabilitata
  //---------------------------------------------
//if(break_Com[Port]) return 0;  // COMMENTATO PER LASCIARE PIU' LIBERTA' ALLE GESTIONI ESTERNE DELLE COMUNICAZIONI

  //---------------------------------------------
  // Comunicazione HOST LINK
  //---------------------------------------------
  if(Tipo==0){
   //if( break_plc ) return 0;
     for(rip1=0;rip1<fact_err;rip1++){
        err = load_canale(OmrPlc[plc].Com, canale, IdPlc, cmd, canale_ini, num_canali, all_mess);
        if(err == 0)break;
     }
  }
  //---------------------------------------------
  // Comunicazione FINS PROTOCOL
  //---------------------------------------------
  if(Tipo==1){
     //---------------------------------------------
     // Inizializzazzione variabili per protocollo
     // FINS
     //---------------------------------------------
     TipoComando = 1;     // lettura.
     if(strcmp(cmd, "RD")==0){TipoVariabile=1;}
     if(strcmp(cmd, "RR")==0){TipoVariabile=2;}
     if(strcmp(cmd, "RW")==0){TipoVariabile=3;}
     if(strcmp(cmd, "RH")==0){TipoVariabile=4;}
     if(strcmp(cmd, "RL")==0){TipoVariabile=2; canale_ini+=(WORD)(1000);}
     if(strcmp(cmd, "RJ")==0){TipoVariabile=5;}
     if(strcmp(cmd, "R#")==0){TipoVariabile=6;}
     if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
     if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
     if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
     if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
     if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
     if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
     if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
     if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
     if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
     if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
     if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
     if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
     if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
     AttesaRisposta=1;
     err = (WORD) OmrLeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  }
  //---------------------------------------------
  // Comunicazione ALGO COMUNICATION
  //---------------------------------------------
  if(Tipo==2){
     err = Scambio_ld_canale(canale, plc, cmd, canale_ini, num_canali, all_mess);
  }
  //---------------------------------------------
  // Comunicazione ETHERNET-HOST LINK
  //---------------------------------------------
  if(Tipo==3){
     //-------------------------------
     // traccia
     //-------------------------------
     if(OmrEth_MultiPlcEsclusionePLC[plc]==true && OmrEthSer_ErroriConsecutivi[plc]>=ETHSER_MAX_ERRORICONSECUTIVI){
        sprintf(all_mess, "disabilitazione temporanea (plc:%d - err:%d)", plc, OmrEthSer_ErroriConsecutivi[plc]);
        file_cronologico_generico("multiplc.txt", all_mess, 30000L);
     }
     //----------------------------------------------------------------------
     // Inizializzazzione variabili per protocollo Ethernet-Seriale
     // Fai 2 tentativi se errore<>3 ( cioè non in fase di sincronizzazione
     //----------------------------------------------------------------------
     err = OmrEthSer_LoadCanale( canale, plc, cmd, canale_ini, num_canali, all_mess);
     if( err && err!=3 ) err = OmrEthSer_LoadCanale( canale, plc, cmd, canale_ini, num_canali, all_mess);
     //strcpy(all_mess, &OmrEthSer_MessaggioAllarmePlc[plc][0]);
     //-------------------------------
     // traccia
     //-------------------------------
     if(OmrEth_MultiPlcEsclusionePLC[plc]==true && err==0){
        sprintf(all_mess, "abilitazione (plc:%d - err:%d)", plc, OmrEthSer_ErroriConsecutivi[plc]);
        file_cronologico_generico("multiplc.txt", all_mess, 30000L);
     }
     //----------------------------------------------------------------------
     // - Se la comunicazione è andata a buon fine azzero il conteggio
     //   degli errori consecutivie e ripristino l'abilitazione al
     //   MULTI PLC se la gestione è attivata
     // - Se comunque non si ripristina la comunizazione azzero gli
     //   errori consecutivi
     //----------------------------------------------------------------------
     if(err==0 || OmrEthSer_ErroriConsecutivi[plc]>ETHSER_MAX_ERRORICONSECUTIVI+100){
        OmrEthSer_ErroriConsecutivi[plc]=0;
        if(AbilitaLetturaMultiPLC==true) OmrEth_MultiPlcEsclusionePLC[plc]=false;
     }
  }
  //---------------------------------------------
  // Comunicazione FINS ETHERNET-TCP
  //---------------------------------------------
  if(Tipo==4){
     //---------------------------------------------
     // Inizializzazzione variabili per protocollo
     // FINS
     //---------------------------------------------
     TipoComando = 1;     // lettura.
     if(strcmp(cmd, "RD")==0){TipoVariabile=1;}
     if(strcmp(cmd, "RR")==0){TipoVariabile=2;}
     if(strcmp(cmd, "RW")==0){TipoVariabile=3;}
     if(strcmp(cmd, "RH")==0){TipoVariabile=4;}
     if(strcmp(cmd, "RL")==0){TipoVariabile=2; canale_ini+=(WORD)(1000);}
     if(strcmp(cmd, "RJ")==0){TipoVariabile=5;}
     if(strcmp(cmd, "R#")==0){TipoVariabile=6;}
     if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
     if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
     if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
     if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
     if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
     if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
     if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
     if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
     if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
     if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
     if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
     if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
     if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
     AttesaRisposta=1;
     //--------------------------------------------------------------------------------
     // 18/11/2005 Fai 2 tentativi se errore<>3 ( cioè non in fase di sincronizzazione
     //--------------------------------------------------------------------------------
     err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
     if( err && err!=3 ) err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
     if(err==0 && AbilitaLetturaMultiPLC==true) OmrEth_MultiPlcEsclusionePLC[plc]=false;
  }
  //---------------------------------------------
  // Gestione degli errori
  //---------------------------------------------
  if( !err ) errore_comunicazione[Port][plc]=0;
  if( err  ) errore_comunicazione[Port][plc]=1;  // Flag che indica un errore comunicazione
  return err;
}


// ------------------------------------------
//      wr_canale()
// ------------------------------------------
// Scrive una sequenza di canali per un massimo di 25 canali. In caso di
// errore ripete la trasmissione e restituisce l'errore dopo fact_err
// ripetizioni.
//
WORD omron::wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  WORD err=0;
  WORD rip;
  WORD TipoComando;
  WORD TipoVariabile;
//  WORD AttesaRisposta;
  WORD Port;
  WORD IdPlc;
  int  Tipo;

  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
  IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Selezione porta di comunicazione
  //---------------------------------------------
  Port=COM1;                             // TGVs
  if(plc==0               ) Port=COM2;   // PLC TERRA
  if(plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //---------------------------------------------
  // Verifico se la comunicazione è disabilitata
  //---------------------------------------------
//if(break_Com[Port]) return 0;  // COMMENTATO PER LASCIARE PIU' LIBERTA' ALLE GESTIONI ESTERNE DELLE COMUNICAZIONI
  //---------------------------------------------
  // Comunicazione HOST LINK
  //---------------------------------------------
  if(Tipo==0){
     rip=0;
     do{
        err = write_canale(OmrPlc[plc].Com, canale, IdPlc, cmd, canale_ini, num_canali, all_mess);
        if( err ) modo(OmrPlc[plc].Com, IdPlc, 2, all_mess);
        rip ++;
     }while((err !=0 ) && (rip < fact_err));
  }
  //---------------------------------------------
  // Comunicazione FINS PROTOCOL
  //---------------------------------------------
  if(Tipo==1){
     //---------------------------------------------
     // Inizializzazzione variabili per protocollo
     // FINS
     //---------------------------------------------
     TipoComando = 2;     // scrittura.
     if(strcmp(cmd, "WD")==0){TipoVariabile=1;}
     if(strcmp(cmd, "WR")==0){TipoVariabile=2;}
     if(strcmp(cmd, "WW")==0){TipoVariabile=3;}
     if(strcmp(cmd, "WH")==0){TipoVariabile=4;}
     if(strcmp(cmd, "WL")==0){TipoVariabile=2; canale_ini+=(WORD)(1000);}
     if(strcmp(cmd, "WJ")==0){TipoVariabile=5;}
     if(strcmp(cmd, "W#")==0){TipoVariabile=6;}
     if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
     if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
     if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
     if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
     if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
     if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
     if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
     if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
     if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
     if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
     if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
     if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
     if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
     //AttesaRisposta=1;
     err = (WORD) OmrLeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  }
  //---------------------------------------------
  // Comunicazione ALGO COMUNICATION
  //---------------------------------------------
  if(Tipo==2){
     err = Scambio_wr_canale( canale, plc, cmd, canale_ini, num_canali, all_mess);
  }
  //---------------------------------------------
  // Comunicazione ETHERNET-HOST LINK
  //---------------------------------------------
  if(Tipo==3){
     //--------------------------------------------------------------
     // Inizializzazzione variabili per protocollo Ethernet-Seriale
     //--------------------------------------------------------------
     err = OmrEthSer_WriteCanale( canale, plc, cmd, canale_ini, num_canali, all_mess);
     if( err && err!=3 ) err = OmrEthSer_WriteCanale( canale, plc, cmd, canale_ini, num_canali, all_mess);
     //strcpy(all_mess, &OmrEthSer_MessaggioAllarmePlc[plc][0]);
  }
  //---------------------------------------------
  // Comunicazione FINS ETHERNET-TCP
  //---------------------------------------------
  if(Tipo==4){
     //---------------------------------------------
     // Inizializzazzione variabili per protocollo
     // FINS
     //---------------------------------------------
     TipoComando = 2;     // scrittura.
     if(strcmp(cmd, "WD")==0){TipoVariabile=1;}
     if(strcmp(cmd, "WR")==0){TipoVariabile=2;}
     if(strcmp(cmd, "WW")==0){TipoVariabile=3;}
     if(strcmp(cmd, "WH")==0){TipoVariabile=4;}
     if(strcmp(cmd, "WL")==0){TipoVariabile=2; canale_ini+=(WORD)(1000);}
     if(strcmp(cmd, "WJ")==0){TipoVariabile=5;}
     if(strcmp(cmd, "W#")==0){TipoVariabile=6;}
     if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
     if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
     if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
     if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
     if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
     if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
     if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
     if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
     if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
     if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
     if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
     if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
     if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
     //AttesaRisposta=1;
     //--------------------------------------------------------------------------------
     // 18/11/2005 Fai 2 tentativi se errore<>3 ( cioè non in fase di sincronizzazione
     //--------------------------------------------------------------------------------
     err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
     if( err && err!=3 ) err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  }
  //---------------------------------------------
  // Gestione degli errori
  //---------------------------------------------
  if( !err && AttesaRisposta>0 ) errore_comunicazione[Port][plc]=0;
  if( err                      ) errore_comunicazione[Port][plc]=1;  // Flag che indica un errore comunicazione
  return err;
}

// ------------------------------------------
//      wr_canale_NoAttesaRisposta()
// ------------------------------------------
// Scrive una sequenza di canali per un massimo di 25 canali. In caso di
// errore ripete la trasmissione e restituisce l'errore dopo fact_err
// ripetizioni.
//
WORD omron::wr_canale_NoAttesaRisposta(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, WORD NoAttesa, char *all_mess)
/*******************************************************/
{
  WORD err=0;

  err=0;
  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Indice del Plc
  //---------------------------------------------
  if( NoAttesa ) AttesaRisposta=0;
  err = wr_canale( canale, plc, cmd, canale_ini, num_canali, all_mess);
  AttesaRisposta=1;

  return err;
}


// ------------------------------------------
//      long_ld_canale()
// ------------------------------------------
// Legge una sequenza di canali per un massimo di 700 canali.
// n.b. questa funzione ha significato solo per comunicazioni Ethernet
//
WORD omron::long_ld_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  WORD err=0;
  WORD TipoComando;
  WORD TipoVariabile;
  WORD AttesaRisposta;
//WORD IdPlc;
  WORD Port;
  int  Tipo;

  err=0;
  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
//IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Selezione porta di comunicazione
  //---------------------------------------------
  Port=COM1;                             // TGVs
  if(plc==0               ) Port=COM2;   // PLC TERRA
  if(plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //---------------------------------------------
  // comunicazione HOST LINK --> Skip!!
  //---------------------------------------------
  if(Tipo==0){
     err=1;
     sprintf(all_mess, "Error in long_ld_canale(): HOST LINK not possible for PLC%d", plc);
  }
  //---------------------------------------------
  // blocco la lettura di più di 500 DM alla
  // volta
  //---------------------------------------------
  if(num_canali>700){
     err=1;
     sprintf(all_mess, "Error in long_ld_canale(): Too DM requested for PLC%d", plc);
  }
  //---------------------------------------------
  // Inizializzazzione variabili per protocollo
  // FINS
  //---------------------------------------------
  TipoComando = 1;  // lettura.
  if(strcmp(cmd, "RD")==0){TipoVariabile=1;}
  if(strcmp(cmd, "RR")==0){TipoVariabile=2;}
  if(strcmp(cmd, "RW")==0){TipoVariabile=3;}
  if(strcmp(cmd, "RH")==0){TipoVariabile=4;}
  if(strcmp(cmd, "RL")==0){TipoVariabile=2; canale_ini+=(WORD)(1000);}
  if(strcmp(cmd, "RJ")==0){TipoVariabile=5;}
  if(strcmp(cmd, "R#")==0){TipoVariabile=6;}
  if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
  if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
  if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
  if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
  if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
  if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
  if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
  if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
  if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
  if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
  if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
  if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
  if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
  AttesaRisposta=1;
  if( !err && Tipo==1 ) err = (WORD) OmrLeggiScriviCanale       ( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  if( !err && Tipo==4 ) err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  //---------------------------------------------
  // Gestione degli errori
  //---------------------------------------------
  if( !err ) errore_comunicazione[Port][plc]=0;
  if( err  ) errore_comunicazione[Port][plc]=1;  // Flag che indica un errore comunicazione
  return err;
}


// ------------------------------------------
//      long_wr_canale()
// ------------------------------------------
// Scrive una sequenza di canali per un massimo di 700 canali.
// n.b. questa funzione ha significato solo per comunicazioni Ethernet
//
WORD omron::long_wr_canale(WORD *canale, WORD plc, char *cmd, WORD canale_ini, WORD num_canali, char *all_mess)
/*******************************************************/
{
  WORD err=0;
  WORD TipoComando;
  WORD TipoVariabile;
  WORD AttesaRisposta;
  WORD Port;
//WORD IdPlc;
  int  Tipo;

  err=0;
  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
//IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Selezione porta di comunicazione
  //---------------------------------------------
  Port=COM1;                             // TGVs
  if(plc==0               ) Port=COM2;   // PLC TERRA
  if(plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //---------------------------------------------
  // comunicazione HOST LINK --> Skip!!
  //---------------------------------------------
  if(Tipo==0){
     err=1;
     sprintf(all_mess, "Error in long_wr_canale(): HOST LINK not possible for PLC%d", plc);
  }
  //---------------------------------------------
  // blocco la lettura di più di 500 DM alla
  // volta
  //---------------------------------------------
  if(num_canali>700){
     err=1;
     sprintf(all_mess, "Error in long_wr_canale(): Too DM requested for PLC%d", plc);
  }
  //---------------------------------------------
  // Inizializzazzione variabili per protocollo
  // FINS
  //---------------------------------------------
  TipoComando = 2;  // scrittura.
  if(strcmp(cmd, "WD")==0){TipoVariabile=1;}
  if(strcmp(cmd, "WR")==0){TipoVariabile=2;}
  if(strcmp(cmd, "WW")==0){TipoVariabile=3;}
  if(strcmp(cmd, "WH")==0){TipoVariabile=4;}
  if(strcmp(cmd, "WL")==0){TipoVariabile=2; canale_ini+=(WORD)(1000);}
  if(strcmp(cmd, "WJ")==0){TipoVariabile=5;}
  if(strcmp(cmd, "W#")==0){TipoVariabile=6;}
  if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
  if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
  if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
  if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
  if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
  if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
  if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
  if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
  if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
  if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
  if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
  if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
  if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
  AttesaRisposta=1;
  if( !err && Tipo==1 ) err = (WORD) OmrLeggiScriviCanale       ( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  if( !err && Tipo==4 ) err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, canale_ini, num_canali, canale, AttesaRisposta, all_mess);
  //---------------------------------------------
  // Gestione delle porte di comunicazione
  //---------------------------------------------
  if( !err && AttesaRisposta>0 ) errore_comunicazione[Port][plc]=0;
  if( err                      ) errore_comunicazione[Port][plc]=1;  // Flag che indica un errore comunicazione
  return err;
}

// ------------------------------------------
//      ld_TIM_CNT()
// ------------------------------------------
// Legge il valore di predisposizione di un TIMER o di un CONTATORE
// TIM-, TIMH, CNT-, CNTH. In caso di errore ripete la trasmissione
// e restituisce l'errore dopo fact_err ripetizioni.
//
WORD omron::ld_TIM_CNT(WORD *SV, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess)
/*******************************************************/
{
  WORD err=0;
  WORD rip1;
  WORD TipoComando;
  WORD TipoVariabile;
  WORD AttesaRisposta;
  WORD Port;
  WORD IdPlc;
  int  Tipo;

  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
  IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Selezione porta di comunicazione
  //---------------------------------------------
  Port=COM1;                             // TGVs
  if(plc==0               ) Port=COM2;   // PLC TERRA
  if(plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //---------------------------------------------
  // Comunicazione HOST LINK
  //---------------------------------------------
  if(Tipo==0){
     rip1=0;
     for(rip1=0;rip1< fact_err;rip1++){
        err = load_TIM_CNT(OmrPlc[plc].Com, SV, IdPlc, cmd, tipo, numero, all_mess);
        if( err == 0)break;
     }
  }
  if(Tipo==3){
     err = OmrEthSer_Load_TIM_CNT( SV, plc, cmd, tipo, numero, all_mess);
  }
  if(Tipo==1 || Tipo==4 ){
     //---------------------------------------------
     // Inizializzazzione variabili per protocollo
     // FINS
     //---------------------------------------------
     TipoComando = 1;     // lettura.
     if(strcmp(cmd, "RD")==0){TipoVariabile=1;}
     if(strcmp(cmd, "RR")==0){TipoVariabile=2;}
     if(strcmp(cmd, "RW")==0){TipoVariabile=3;}
     if(strcmp(cmd, "RH")==0){TipoVariabile=4;}
     if(strcmp(cmd, "RJ")==0){TipoVariabile=5;}
     if(strcmp(cmd, "R#")==0){TipoVariabile=6;}
     if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
     if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
     if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
     if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
     if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
     if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
     if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
     if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
     if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
     if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
     if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
     if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
     if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
     AttesaRisposta=1;
     if( Tipo==1 ) err = (WORD) OmrLeggiScriviCanale       ( plc, TipoVariabile, TipoComando, numero, 1, SV, AttesaRisposta, all_mess);
     if( Tipo==4 ) err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, numero, 1, SV, AttesaRisposta, all_mess);
  }
  //---------------------------------------------
  // Gestione degli errori
  //---------------------------------------------
  if( !err ) errore_comunicazione[Port][plc]=0;
  if( err  ) errore_comunicazione[Port][plc]=1;  // Flag che indica un errore comunicazione
  return err;
}



// ------------------------------------------
//      wr_TIM_CNT()
// ------------------------------------------
// Sovrascrive il valore di predisposizione di un TIMER o di un CONTATORE
// TIM-, TIMH, NCT-, CNTH in caso di errore ripete la trasmissione e
// restituisce l'errore dopo fact_err ripetizioni.
//
WORD omron::wr_TIM_CNT(WORD *SV, WORD plc, char *cmd, char *tipo, WORD numero, char *all_mess)
/*******************************************************/
{
  WORD err=0;
  WORD rip;
  WORD TipoComando;
  WORD TipoVariabile;
  WORD AttesaRisposta;
  WORD Port;
  WORD IdPlc;
  int  Tipo;

  //---------------------------------------------
  // Recupero i parametri di comunicazione:
  // 1 - Tipo di comunicazione  (HOST/FINS)
  // 2 - Porta di comunicazione (se Host)
  // 3 - Indice del Plc
  //---------------------------------------------
  Tipo  = OmrPlc[plc].TypCom;
  Port  = OmrPlc[plc].Com;
  IdPlc = OmrPlc[plc].IndexPlc;
  //---------------------------------------------
  // Selezione porta di comunicazione
  //---------------------------------------------
  Port=COM1;                             // TGVs
  if(plc==0               ) Port=COM2;   // PLC TERRA
  if(plc==PLC_IO_REMOTO_01) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_02) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_03) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_04) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_05) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_06) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_07) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_08) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_09) Port=COM6;   // I/O REMOTI
  if(plc==PLC_IO_REMOTO_10) Port=COM6;   // I/O REMOTI
  //---------------------------------------------
  // Comunicazione HOST LINK
  //---------------------------------------------
  if(Tipo==0){
     rip=0;
     do{
        //modo(port, plc, 2, all_mess);
        err = write_TIM_CNT(OmrPlc[plc].Com, SV, IdPlc, cmd, tipo, numero, all_mess);
        if( err ) modo(OmrPlc[plc].Com, IdPlc, 2, all_mess);
        rip ++;
     }while((err !=0 ) && (rip < fact_err));
  }
  if(Tipo==3){
     err = OmrEthSer_Write_TIM_CNT( SV, plc, cmd, tipo, numero, all_mess);
  }
  if(Tipo==1 || Tipo==4 ){
     //---------------------------------------------
     // Inizializzazzione variabili per protocollo
     // FINS
     //---------------------------------------------
     TipoComando = 2;     // scrittura.
     if(strcmp(cmd, "WD")==0){TipoVariabile=1;}
     if(strcmp(cmd, "WR")==0){TipoVariabile=2;}
     if(strcmp(cmd, "WW")==0){TipoVariabile=3;}
     if(strcmp(cmd, "WH")==0){TipoVariabile=4;}
     if(strcmp(cmd, "WJ")==0){TipoVariabile=5;}
     if(strcmp(cmd, "W#")==0){TipoVariabile=6;}
     if(strcmp(cmd, "E0")==0){TipoVariabile=7;}  // canali EM block 0
     if(strcmp(cmd, "E1")==0){TipoVariabile=8;}  // canali EM block 1
     if(strcmp(cmd, "E2")==0){TipoVariabile=9;}  // canali EM block 2
     if(strcmp(cmd, "E3")==0){TipoVariabile=10;} // canali EM block 3
     if(strcmp(cmd, "E4")==0){TipoVariabile=11;} // canali EM block 4
     if(strcmp(cmd, "E5")==0){TipoVariabile=12;} // canali EM block 5
     if(strcmp(cmd, "E6")==0){TipoVariabile=13;} // canali EM block 6
     if(strcmp(cmd, "E7")==0){TipoVariabile=14;} // canali EM block 7
     if(strcmp(cmd, "E8")==0){TipoVariabile=15;} // canali EM block 8
     if(strcmp(cmd, "E9")==0){TipoVariabile=16;} // canali EM block 9
     if(strcmp(cmd, "EA")==0){TipoVariabile=17;} // canali EM block A
     if(strcmp(cmd, "EB")==0){TipoVariabile=18;} // canali EM block B
     if(strcmp(cmd, "EC")==0){TipoVariabile=19;} // canali EM block C
     AttesaRisposta=1;
     if( Tipo==1 ) err = (WORD) OmrLeggiScriviCanale       ( plc, TipoVariabile, TipoComando, numero, 1, SV, AttesaRisposta, all_mess);
     if( Tipo==4 ) err = (WORD) OmrEthTcp_LeggiScriviCanale( plc, TipoVariabile, TipoComando, numero, 1, SV, AttesaRisposta, all_mess);
  }
  //---------------------------------------------
  // Gestione degli errori
  //---------------------------------------------
  if( !err && AttesaRisposta>0 ) errore_comunicazione[Port][plc]=0;
  if( err                      ) errore_comunicazione[Port][plc]=1;  // Flag che indica un errore comunicazione
  return err;
}

// -----------------------------------
//    MODBUS_RD_Registri()
// -----------------------------------
// Emula la funzione 03 del protocollo MODBUS per la lettura di registri
//
//
int MODBUS_RD_Registri( short int Port, short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess )
// *********************************
{
  int err = 0;
  int Com = COM_SIEMENS;

  //------------------------------------------------------------
  // Inizializzazione variabile
  //------------------------------------------------------------
  errore_comunicazione[Com][NumPlc]=0;
  //------------------------------------------------------------
  // Funzione di comunizazione globale
  //------------------------------------------------------------
  err = MODBUS_Leggi_Registri(Port, NumPlc, IndIni, NumRegistri, Registri, all_mess );
  //------------------------------------------------------------
  // Dopo due tentativi se persiste l'errore
  // --> skip!
  //------------------------------------------------------------
  errore_comunicazione[Com][NumPlc]=(WORD)(err);

  return err;
}

// -----------------------------------
//    MODBUS_WR_Registri()
// -----------------------------------
// Emula la funzione 16 del protocollo MODBUS per la scrittura di registri
//
//
int MODBUS_WR_Registri( short int Port, short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess )
// *********************************
{
  int err = 0;
  int Com = COM_SIEMENS;

  //------------------------------------------------------------
  // Inizializzazione variabile
  //------------------------------------------------------------
  errore_comunicazione[Com][NumPlc]=0;
  //------------------------------------------------------------
  // Funzione di comunizazione globale (da libreria)
  //------------------------------------------------------------
  err = MODBUS_Scrivi_Registri(Port, NumPlc, IndIni, NumRegistri, Registri, all_mess );
  //--------------------------------------------
  // Dopo due tentativi se persiste l'errore
  // --> skip!
  //--------------------------------------------
  errore_comunicazione[Com][NumPlc]=(WORD)(err);
  return err;
}
//-----------------------------------------------------------------------
// -----------------------------------
//    MODBUS_RD_Registri_Tcp()
// -----------------------------------
// Emula la funzione 03 del protocollo MODBUS per la lettura di registri
//
//
int MODBUS_RD_Registri_Tcp(short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess )
// *********************************
{
  int err=0;
  int Com = COM_SIEMENS;

  //------------------------------------------------------------
  // Funzione di comunizazione globale (da libreria)
  //------------------------------------------------------------
  err = MODBUS_Leggi_Registri_Tcp(NumPlc, IndIni, NumRegistri, Registri, all_mess );
  //------------------------------------------------------------
  // Errori di comunicazione
  //------------------------------------------------------------
  if( !err ) errore_comunicazione[Com][NumPlc]=0;
  if( err  ){
     errore_comunicazione[Com][NumPlc]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumPlc]>100) errore_comunicazione[Com][NumPlc]=1;
  }

  return err;
}

// -----------------------------------
//    MODBUS_WR_Registri_Tcp()
// -----------------------------------
// Emula la funzione 16 del protocollo MODBUS per la scrittura di registri
//
//
int MODBUS_WR_Registri_Tcp(short int NumPlc, short int IndIni, short int NumRegistri, unsigned short int* Registri, char* all_mess )
// *********************************
{
  int err=0;
  int Com = COM_SIEMENS;

  //------------------------------------------------------------
  // Funzione di comunizazione globale (da libreria)
  //------------------------------------------------------------
  err = MODBUS_Scrivi_Registri_Tcp(NumPlc, IndIni, NumRegistri, Registri, all_mess);
  //------------------------------------------------------------
  // Errori di comunicazione
  //------------------------------------------------------------
  if( !err ) errore_comunicazione[Com][NumPlc]=0;
  if( err  ){
     errore_comunicazione[Com][NumPlc]++;  // Flag che indica un errore comunicazione
     if(errore_comunicazione[Com][NumPlc]>100) errore_comunicazione[Com][NumPlc]=1;
  }


  return err;
}

//---------------------------------------------------------------------------
// delay_speciale()
// Attendi per x msec processando tutti i messaggi di WINDOWS
//---------------------------------------------------------------------------
void delay_speciale( int MsecAttesa)
{
  SYSTEMTIME TimeInizio;
  SYSTEMTIME TimeFine;
  int WaitReponse;
  int msec;

  WaitReponse=0;
  GetLocalTime( &TimeInizio );
  for( ;; ){
     Application->ProcessMessages();
     if (Application->Terminated)   break;
     //------------------------
     // Controllo TIME_OUT
     //------------------------
     if( WaitReponse>=1000 ){
        GetLocalTime( &TimeFine );
        msec = (int) (TimeFine.wSecond - TimeInizio.wSecond);
        if( msec<0 ) msec+=60;
        msec = msec * 1000;
        msec = msec + (int) (TimeFine.wMilliseconds - TimeInizio.wMilliseconds);
        // Verifica che il ritardo non superi TimeOut
        if( msec > MsecAttesa ) break;
        WaitReponse=0;
     }
     WaitReponse++;
  }
  return ;
}


//---------------------------------------------------------------------------
// Funzione di radice quadrata controllata
//---------------------------------------------------------------------------
double sqrt_algo( double Variabile)
{
//char   buffer[300];
  double Result=0;
  double MaxVal=1.0E100; //Un double va da 1*10e-308 a1*10e+308 ....
//int err=0;

  //-----------------------------------------------------
  // Controllo dell'errore per test sqrt()
  // Valori negativi sicuramente danno errori di sqrt()
  // forse nelle operazione sqrt( int*int + int*int )
  //-----------------------------------------------------
  if( Variabile<0        ) {
//   err=1;
//   sprintf( buffer, "sqrt_algo() error : valore Variabile negativo %.0f "        ,  Variabile);
     Variabile=Variabile*-1;
  }
  if( Variabile>MaxVal   ) {
//   err=2;
//   sprintf( buffer, "sqrt_algo() error : valore Variabile supera range  (Max=%.0f)",   MaxVal);
     Variabile=MaxVal;
  }
//  buffer[80]=0;
//  if( err    )  P.error_trasmit( 0, buffer);

  Result = sqrt( Variabile );

  return Result;
}



