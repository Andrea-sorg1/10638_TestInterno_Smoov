//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>

#include <fcntl.h>
#include <errno.h>
#include <share.h>
#include <sys\stat.h>

#pragma hdrstop

#include "Main.h"
#include "Stato.h"
#include "Layout.h"
#include "AgvWin.h"
#include "Remoto.h"
#include "VisFile.h"
#include "Listagen.h"
#include "OmronETH.h"
#include "TecnoFer.h"
#include "AllenBViaVbOpc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Ping"
#pragma resource "*.dfm"
TFormStato *FormStato;
//----------------------------------------------------
// Variabili definite in GRAPHVAR.CPP
//----------------------------------------------------
extern int   offset[][2];
extern float Rap;
// ---------------------
//   STRUTTURA NODI
// ---------------------
//0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];
//-------------------------------------------------------


//---------------------------------------------------------------------------
__fastcall TFormStato::TFormStato(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::FormCreate(TObject *Sender)
{
  int i;

  //----------------------------------------------------------
  // Posizionamento del PageControl della Form di stato sul
  // primo TabSheet
  //----------------------------------------------------------
  NumeroTGV=1;
  PageControlTGV->ActivePage = FormStato->TabTGV1;
  ShColoreTGV->Brush->Color = AGV[NumeroTGV]->mission.Col;
  //----------------------------------------------------------
  // Associazione degli oggetti a un array
  //----------------------------------------------------------
  TabTGV[ 1] = TabTGV1;
  TabTGV[ 2] = TabTGV2;
  TabTGV[ 3] = TabTGV3;
  TabTGV[ 4] = TabTGV4;
  TabTGV[ 5] = TabTGV5;
  TabTGV[ 6] = TabTGV6;
  TabTGV[ 7] = TabTGV7;
  TabTGV[ 8] = TabTGV8;
  TabTGV[ 9] = TabTGV9;
  TabTGV[10] = TabTGV10;
  TabTGV[11] = TabTGV11;
  TabTGV[12] = TabTGV12;
  TabTGV[13] = TabTGV13;
  TabTGV[14] = TabTGV14;
  TabTGV[15] = TabTGV15;
  TabTGV[16] = TabTGV16;
  TabTGV[17] = TabTGV17;
  TabTGV[18] = TabTGV18;
  TabTGV[19] = TabTGV19;
  TabTGV[20] = TabTGV20;
  TabTGV[21] = TabTGV21;
  //----------------------------------------------------------
  // Disattivazione dei TabSheet per il TGV inesistenti
  //----------------------------------------------------------
  for(i=MAXAGV+1; i<22; i++) TabTGV[i]->TabVisible = false;
  //------------------------------------------------------
  // Recupero il LOGO DEL'IMPIANTO
  //------------------------------------------------------
  ImageLogo->Picture->LoadFromFile("Grafica\\LogoImpianto.bmp");
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::FormActivate(TObject *Sender)
{
  //------------------------------------------------------
  // Caricamento della messaggeria
  //------------------------------------------------------
  Messaggi();
  //------------------------------------------------------
//  if(GestioneUdc>0) gbTools->Visible=true;
//  else gbTools->Visible=false;
  if(GestioneUdc>0){
     btnSearchUDC->Visible=true;
     SpeedButton2->Visible=true;
     SpeedButton3->Visible=true;
     SpeedButton4->Visible=true;
     btnAttivaFunzSpec->Visible=true;
  }
  else{
     btnSearchUDC->Visible=false;
     SpeedButton2->Visible=false;
     SpeedButton3->Visible=false;
     SpeedButton4->Visible=false;
     btnAttivaFunzSpec->Visible=false;
  }
  Panel2->Top=595;
  //------------------------------------------------------
  if(PcAlgo==true) btnAttivaFunzSpec->Enabled=true;
  else btnAttivaFunzSpec->Enabled=false;
  //------------------------------------------------------
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::Messaggi()
{
  // ---  Pagina laterale stato TGV  ---
  GroupBox2->Caption    = MESS[540];
  LabStat1->Caption     = MESS[541];
  LabStat2->Caption     = MESS[542];
  LabStat3->Caption     = MESS[543];
  LabStat4->Caption     = MESS[544];
  LabStat5->Caption     = MESS[545];
  BtnReset1->Caption    = MESS[546];
  GroupBox1->Caption    = MESS[547];
  GroupBox3->Caption    = MESS[548];
  BoxPriorita->Caption  = MESS[550];
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::BtnEditorClick(TObject *Sender)
{
  //----------------------------------------------
  // Apertura dell'editor del TGV selezionato
  //----------------------------------------------
  FormTgv->NumeroTgv = NumeroTGV;
  FormTgv->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::BtnPercorsoClick(TObject *Sender)
{
  int err;
  char all_mess[101]="";
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, "PERCTGV.DAT");
  if(AgvBloccati[NumeroTGV]==0) sprintf(all_mess, "Path TGV%d", NumeroTGV);
  else                          sprintf(all_mess, "Path TGV%d (TGV%d in obstruction)", NumeroTGV, AgvBloccati[NumeroTGV]);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, all_mess);
  err=AGV[NumeroTGV]->CompilaPercorsoGrafico( FormVisualizzaFile->IntestazioneFileVisualizza );
  if(!err) FormVisualizzaFile->ShowModal();
  else{
     sprintf(all_mess, MESS[113], FormVisualizzaFile->NomeFileVisualizza);
     Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::BtnProgrPLCClick(TObject *Sender)
{
  int err;
  char all_mess[101]="";
  //-----------------------------------------------------
  // Settaggio variabili per visualizzazione File
  //-----------------------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, "PROGTGV.DAT");
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, "| STEP  |   ISTRUCTION                         |   CODING OF CONTROLS          |");
  sprintf(all_mess, "Program TGV%d - (PLC)", NumeroTGV);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, all_mess);
  err=AGV[NumeroTGV]->CompilaProgrammaGrafico( TRUE );
  if(!err) FormVisualizzaFile->ShowModal();
  else{
     sprintf(all_mess, MESS[113], FormVisualizzaFile->NomeFileVisualizza);
     Application->MessageBox(all_mess, MESS[100], MB_OK | MB_ICONEXCLAMATION);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::BtnMissioniClick(TObject *Sender)
{
  char NomeFile1[200];
  char NomeFile2[200];

  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  sprintf( NomeFile1, "%s\\HIST%03d.DAT", DriveServer, NumeroTGV);
  sprintf( NomeFile2, "HIST%03d.DAT", NumeroTGV);
  if( IdPC>1 ){
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  //-----------------------------------------
  //  Apertura dei files da SERVER
  //-----------------------------------------
  strcpy(FormVisualizzaFile->NomeFileVisualizza, NomeFile2);
  strcpy(FormVisualizzaFile->CaptionFileVisualizza, MESS[367]);
  strcpy(FormVisualizzaFile->IntestazioneFileVisualizza, MESS[645]);
  FormVisualizzaFile->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::BtnCronoAllClick(TObject *Sender)
{
  char NomeFile1[200];
  char NomeFile2[200];
/*****************
  //-----------------------------------------
  //  Recupera file da SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile1, "%s\\CRONOALL.DAT", DriveServer );
     sprintf( NomeFile2, "CRONOALL.DAT" );
     CopyFile( NomeFile1, NomeFile2, FALSE );
  }
  strcpy(FormListagen->NomeFileLista, "CRONOALL.DAT");
  strcpy(FormListagen->CaptionFileLista, "Cronologico allarmi");
  FormListagen->TipoFileLista=3;
  FormListagen->ShowModal();
  *********************************************/
  FormMain->MStoricoAllarmiClick(Sender);

}
//---------------------------------------------------------------------------
void __fastcall TFormStato::PageControlTGVChange(TObject *Sender)
{
  NumeroTGV=1;
  //----------------------------------------
  // Recupero il numero del TGV selezionato
  //----------------------------------------
  if(PageControlTGV->ActivePage==TabTGV1 ) NumeroTGV = 1;
  if(PageControlTGV->ActivePage==TabTGV2 ) NumeroTGV = 2;
  if(PageControlTGV->ActivePage==TabTGV3 ) NumeroTGV = 3;
  if(PageControlTGV->ActivePage==TabTGV4 ) NumeroTGV = 4;
  if(PageControlTGV->ActivePage==TabTGV5 ) NumeroTGV = 5;
  if(PageControlTGV->ActivePage==TabTGV6 ) NumeroTGV = 6;
  if(PageControlTGV->ActivePage==TabTGV7 ) NumeroTGV = 7;
  if(PageControlTGV->ActivePage==TabTGV8 ) NumeroTGV = 8;
  if(PageControlTGV->ActivePage==TabTGV9 ) NumeroTGV = 9;
  if(PageControlTGV->ActivePage==TabTGV10) NumeroTGV = 10;
  if(PageControlTGV->ActivePage==TabTGV11) NumeroTGV = 11;
  if(PageControlTGV->ActivePage==TabTGV12) NumeroTGV = 12;
  if(PageControlTGV->ActivePage==TabTGV13) NumeroTGV = 13;
  if(PageControlTGV->ActivePage==TabTGV14) NumeroTGV = 14;
  if(PageControlTGV->ActivePage==TabTGV15) NumeroTGV = 15;
  if(PageControlTGV->ActivePage==TabTGV16) NumeroTGV = 16;
  if(PageControlTGV->ActivePage==TabTGV17) NumeroTGV = 17;
  if(PageControlTGV->ActivePage==TabTGV18) NumeroTGV = 18;
  if(PageControlTGV->ActivePage==TabTGV19) NumeroTGV = 19;
  if(PageControlTGV->ActivePage==TabTGV20) NumeroTGV = 20;
  if(PageControlTGV->ActivePage==TabTGV21) NumeroTGV = 21;
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::VisualizzaStatoTGV()
{
  int   err=0;
  int   len;
  int   handle;
  int   max_all;
  int   TotMac  = TOTCHIAMATE;
  int   TotBaie = TOTBAIE;
  int   tgv_in_chiamata;
  char  buffer1[101]="";
  char  stringa[501]="";
  char  stringa_allarmi[1501]="";
  char  str_allarmi[MAX_ALLARM][101];                   // Messaggeria Allarmi (navette FORK)
  short int OrdinaMac[MAXCHIAMATE+1];
  short int PrioritaMac[MAXCHIAMATE+1];
  static short int OldPrioritaMac[MAXCHIAMATE+1];
  short int OrdinaBaie[MAXBAIE+1];
  short int PrioritaBaie[MAXBAIE+1];
  static short int OldPrioritaBaie[MAXBAIE+1];
  short int i=0, n=0, j=0;
  WORD  dm[MAXLRALLARMI+2];
  short int num_mac;
  short int num_baia;
  short int num_stz;
  short int num_mag;
  short int num_baia1, num_mac1, num_mag1, num_stz1;
  char  Forca[30]="";
  char  sigla[21]="";
  char  sigla_scaff[51]="";

  memset(&dm[0], 0, sizeof(dm));
  memset(&str_allarmi,     NULL, sizeof(str_allarmi));
  memset(&stringa_allarmi, NULL, sizeof(stringa_allarmi));
  //----------------------------------------
  // Ridimensionamento lista chiamate
  //----------------------------------------
  if(TotMac<1){
     ListPrioritaMac->Visible  = false;
     ListPrioritaBaie->Top     = 20;
     ListPrioritaBaie->Height  = 126;
  }
  if(TotBaie<1){
     ListPrioritaBaie->Visible = false;
     ListPrioritaMac->Top      = 20;
     ListPrioritaMac->Height   = 126;
  }

  //----------------------------------------
  // Associazione del colore allo shape
  //----------------------------------------
  ShColoreTGV->Brush->Color = AGV[NumeroTGV]->mission.Col;
  if(AGV[NumeroTGV]->mission.Esclusione && Lampeggio) ShColoreTGV->Brush->Color=clWhite;
  //----------------------------------------
  // Dati della missione in corso
  //----------------------------------------
  TextPos->Caption    = AGV[NumeroTGV]->stato.pos;
  TextPStart->Caption = AGV[NumeroTGV]->mission.pstart;
  TextPEnd->Caption   = AGV[NumeroTGV]->mission.pend;
  TextPInt->Caption   = AGV[NumeroTGV]->mission.pintend;
  TextPIntF->Caption  = AGV[NumeroTGV]->mission.PIntForzato;
  TextStart->Caption  = AGV[NumeroTGV]->stato.start;
  //----------------------------------------
  // Stato TGV
  //----------------------------------------
  if(!AGV[NumeroTGV]->mission.SemiAutomatico){
     if(!AGV[NumeroTGV]->stato.s.bit.funz ){TextFunz->Caption="M"; TextFunz->Font->Color=clRed;}
     if( AGV[NumeroTGV]->stato.s.bit.funz ){TextFunz->Caption="A"; TextFunz->Font->Color=clAqua;}
  }
  else{                                    //{TextFunz->Caption="S"; TextFunz->Font->Color=clYellow;}
     if(!AGV[NumeroTGV]->stato.s.bit.funz ){TextFunz->Caption="M"; TextFunz->Font->Color=clRed;}
     if( AGV[NumeroTGV]->stato.s.bit.funz ){TextFunz->Caption="S"; TextFunz->Font->Color=clYellow;}
  }
  //----------------------------------------
  // Visualizzazione Allarmi
  //----------------------------------------
  if(!AGV[NumeroTGV]->stato.s.bit.allarme && !AGV[NumeroTGV]->allarme_interno && !AGV[NumeroTGV]->stato.s.bit.ris){
     TextAllarme->Caption = "OFF";
     TextAllarme->Color   = clBtnFace;
  }
  else{
     TextAllarme->Caption = "";
     // --- allarme ---
     if((AGV[NumeroTGV]->stato.s.bit.allarme || AGV[NumeroTGV]->allarme_interno) && Lampeggio){
        TextAllarme->Caption = "ALLARM";
        TextAllarme->Color   = clRed;
     }
     // --- tgv in riserva ---
     if(AGV[NumeroTGV]->stato.s.bit.ris && Lampeggio){
        TextAllarme->Caption = "BATTERY";
        TextAllarme->Color   = (TColor)(0x004080FF);
     }
  }
  //-----------------------------------------------------
  // Chiamata in asserviemtno
  // Carrello a bordo
  //-----------------------------------------------------
  sprintf(stringa, "%06d", AGV[NumeroTGV]->mission.Chiamata);
  TextChiamata->Font->Color = clFuchsia;
  TextChiamata->Caption     = stringa;
  //-----------------------------------------------------
  // Visualizzazione TGV in CAMBIO MISSIONE AL VOLO
  //-----------------------------------------------------
  if(AGV[NumeroTGV]->mission.ResetAlVolo==true && Lampeggio==true){
     TextChiamata->Font->Color = clBlack;
  }
  //-----------------------------------------------------
  // Stato Batterie
  // GESTIONE_ASRV (visualizzazione solo della %)
  //-----------------------------------------------------
  #ifdef GESTIONE_ASRV
     sprintf(stringa, "%d%", AGV[NumeroTGV]->mission.PercCaricaBatt);
     if(AGV[NumeroTGV]->mission.PercCaricaBatt<35) TextBatteria->Color=(TColor)(0x008080FF);   // scarico  (rosso)
     if(AGV[NumeroTGV]->mission.PercCaricaBatt>34 &&
        AGV[NumeroTGV]->mission.PercCaricaBatt<59) TextBatteria->Color=(TColor)(0x0080FFFF);   // riserva  (giallo)
     if(AGV[NumeroTGV]->mission.PercCaricaBatt>58) TextBatteria->Color=(TColor)(0x0080FF80);   // carico   (verde)
  #else
     sprintf(stringa, "%dV - %d%", AGV[NumeroTGV]->mission.VoltBatteria, AGV[NumeroTGV]->mission.PercCaricaBatt);
     if(AGV[NumeroTGV]->mission.VoltBatteria<47) TextBatteria->Color=(TColor)(0x008080FF);   // scarico  (rosso)
     if(AGV[NumeroTGV]->mission.VoltBatteria>46 &&
        AGV[NumeroTGV]->mission.VoltBatteria<49) TextBatteria->Color=(TColor)(0x0080FFFF);   // riserva  (giallo)
     if(AGV[NumeroTGV]->mission.VoltBatteria>48) TextBatteria->Color=(TColor)(0x0080FF80);   // carico   (verde)
  #endif;
  TextBatteria->Caption = stringa;
  //-----------------------------------------------------
  // Gradi di movimentazione TGV
  //-----------------------------------------------------
  if(AGV[NumeroTGV]->mission.GestioneLaser==false){
     LabGrd->Caption   = IntToStr(AGV[NumeroTGV]->stato.rot_calamite) + "°";
  }
  else{
     LabGrd->Caption   = IntToStr(AGV[NumeroTGV]->stato.rot_laser) + "°";
  }
  sprintf(stringa, "%x", AGV[NumeroTGV]->stato.g_laser);
  LabGLaser->Caption   = stringa;
  LabPercSpec->Caption = AGV[NumeroTGV]->PercorsiSpeciali;
  //-----------------------------------------------------
  // Visualizzazione del "G" laser
  //-----------------------------------------------------
  LabGLaser->Visible=false;
  if(AGV[NumeroTGV]->mission.GestioneLaser==true) LabGLaser->Visible=true;
  //-----------------------------------------------------
  // - Zona in cui si muove il TGV
  // - TGV in attesa
  //-----------------------------------------------------
  TextZona->Caption   = AGV[NumeroTGV]->mission.Zona;
  TextAttesa->Caption = (int)(AGV[NumeroTGV]->mission.TGVInCambioMissione);
  //-----------------------------------------------------
  // Compilazione file con Allarmi attuali TGV
  //-----------------------------------------------------
  TextListAll->Caption="";
  if(AGV[NumeroTGV]->stato.s.bit.allarme || AGV[NumeroTGV]->allarme_interno){
     // --- POSSIBILI MESSAGGI DI ALLARME ---
     memcpy(&str_allarmi[0][0], &ALLARM[0][0], sizeof(str_allarmi));
     // --- SERVER/CLIENTs ---
     for( i=0;i<MAXLRALLARMI;i++){
        dm[i] = AGV[NumeroTGV]->allarme_esterno[i];
     }
     //-------------------------------------------------------
     // situazione allarmi da aggiornare
     //-------------------------------------------------------
     n=0;
     //-----------------------------------------------------
     // Visualizzazione allarmi presenti
     //-----------------------------------------------------
     max_all=0;
     strcpy(stringa_allarmi, "");
     for( i=0;i<MAXLRALLARMI;i++){
        if(max_all>5) break;
        for( j=0;j<16;j++){
           if(max_all>5) break;
           if(((dm[i]>>j)&1) == 1 ){
              memcpy(&stringa[0], &str_allarmi[n][0], 63);
              stringa[63]=NULL;
              //---------------------------------------------------
              // Verifica il bit dell'allarme
              //---------------------------------------------------
              Copy(str_allarmi[n], 5,   5, buffer1);
              if(strcmp(buffer1, "     ")==0) continue;
              //---------------------------------------------------
              // Registrazione della stringa
              //---------------------------------------------------
              strcat(stringa_allarmi, stringa);
              strcat(stringa_allarmi, "\n");
              max_all++;     // visualizzo al max 6 allarmi in lista.
           }
           n++;
        }
     }
     //-----------------------------------------------------
     // allarmi interni
     //-----------------------------------------------------
     if( AGV[NumeroTGV]->allarme_interno >0 ) sprintf( stringa_allarmi, "Allarme interno:%d", AGV[NumeroTGV]->allarme_interno );
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_RESET_DATI         ) strcpy(stringa_allarmi, MESS[360]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_SCAR_MANUALE       ) strcpy(stringa_allarmi, MESS[361]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_POS_NOT_OK         ) strcpy(stringa_allarmi, MESS[362]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_STOP_TGV           ) sprintf(stringa_allarmi, "stop TGV! view in menù: (%s)", MESS[42]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_DATI_ANOMALI       ) sprintf(stringa_allarmi, "anomalia stato navetta! view in menù: (%s)", MESS[42]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_ERR_COMUNIC        ) sprintf(stringa_allarmi, "persistent comunication error! view in menù: (%s)", MESS[42]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_RICARICA_OCC       ) strcpy(stringa_allarmi, MESS[340]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_START_DA_OCC       ) strcpy(stringa_allarmi, MESS[379]);
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_ID_NOT_OK          ) strcpy(stringa_allarmi, "Id not ok!");
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_DEST_IMPEGNATA     ) strcpy(stringa_allarmi, "destination not available!");
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_DATI_BOX_NON_VALIDI) strcpy(stringa_allarmi, "pallet data not ok!");
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_UDC_DUPLICATO      ) strcpy(stringa_allarmi, "udc duplicated!");
     if( AGV[NumeroTGV]->allarme_interno == ALL_INT_DEST_NON_VALIDA    ) strcpy(stringa_allarmi, "destination not ok or not exist!");
     //-----------------------------------------------------
     // Visualizzazione stringa complessiva allarmi
     //-----------------------------------------------------
     TextListAll->Caption=stringa_allarmi;
  }
  //------------------------------------------------------
  // Destinazione appartiene Autoclave, Cest, Mag....
  //------------------------------------------------------
  num_baia=num_mac=num_mag=num_stz=0;
  P.test_punto_presente_mac(buffer1, AGV[NumeroTGV]->mission.pend, &num_mac, &num_stz);
  P.test_punto_presente_baia(buffer1, AGV[NumeroTGV]->mission.pend, &num_baia);
  M.test_punto_presente_mag(buffer1, AGV[NumeroTGV]->mission.pend, &num_mag, &num_stz);
  //------------------------------------------------------
  // Scompongo la chiamata per sapere la macchina
  // macchina chiamante
  //------------------------------------------------------
  if(AGV[NumeroTGV]->mission.Chiamata!=0){
     num_baia1=num_mag1=num_mac1=num_stz1=0;
     P.TestChiamataNum(AGV[NumeroTGV]->mission.Chiamata, &num_mag1, &num_baia1, &num_mac1, &num_stz1);
  }
  //------------------------------------------------------
  // Visualizzazione dei dati dei contenitori a bordo TGV
  //------------------------------------------------------
  strcpy(Forca, "");
  strcpy(stringa, "");
  strcpy(sigla_scaff, "");
  if(AGV[NumeroTGV]->mission.pstart!=0){
     //------------------------------------------------------
     // Visualizzazione codifica numerica tipo presa
     //------------------------------------------------------
     sprintf( Forca, "N" );
     if( AGV[NumeroTGV]->mission.TypPresa==N_FORCA_A )  sprintf( Forca, "A" );
     if( AGV[NumeroTGV]->mission.TypPresa==N_FORCA_B )  sprintf( Forca, "B" );
     if( AGV[NumeroTGV]->mission.TypPresa==N_FORCA_AB)  sprintf( Forca, "A+B" );
     //------------------------------------------------------
     // Descrizione missione
     //------------------------------------------------------
     sprintf(stringa, MESS[553], AGV[NumeroTGV]->mission.pend);
     // -- posizionamento semplice
     if(AGV[NumeroTGV]->mission.Typ==MISS_SOLO_POSIZIONAMENTO
        && AGV[NumeroTGV]->mission.PIntForzato==0                   ) sprintf(stringa, MESS[551], AGV[NumeroTGV]->mission.pend);
     // -- avvicinamento
     if(AGV[NumeroTGV]->mission.Typ==MISS_SOLO_POSIZIONAMENTO
        && AGV[NumeroTGV]->mission.PIntForzato!=0                   ) strcpy(stringa,  MESS[552]);
     // -- carico/scarico macchina
     if(num_mac>0 && num_mac<=TOTCHIAMATE) strcpy(sigla, P.ch[num_mac]->Sigla);
     if(AGV[NumeroTGV]->mission.Typ==MISS_CARICO_DA_MACCHINA        ) sprintf(stringa, MESS[554], num_mac, sigla, Forca);
     if(AGV[NumeroTGV]->mission.Typ==MISS_SCARICO_SU_MACCHINA       ) sprintf(stringa, MESS[555], num_mac, sigla, Forca);
     // -- carico/scarico baia
     if(num_baia>0 && num_baia<=TOTBAIE){
        strcpy(sigla, P.ba[num_baia]->Sigla);
     }
     if(AGV[NumeroTGV]->mission.Typ==MISS_CARICO_DA_BAIA            ) sprintf(stringa, MESS[560], num_baia, sigla, Forca);
     if(AGV[NumeroTGV]->mission.Typ==MISS_SCARICO_SU_BAIA           ) sprintf(stringa, MESS[561], num_baia, sigla, Forca);

     // -- carico/scarico magazzino
     if(AGV[NumeroTGV]->mission.Chiamata==0){
        if(AGV[NumeroTGV]->mission.Typ==MISS_CARICO_DA_MAGAZZINO    ) sprintf(stringa, MESS[558], num_mag, Forca);
        if(AGV[NumeroTGV]->mission.Typ==MISS_SCARICO_SU_MAGAZZINO   ) sprintf(stringa, MESS[559], num_mag, Forca);
     }
     else{
        if(num_mac1>0 && num_mac1<=TOTCHIAMATE  ) strcpy(sigla, P.ch[num_mac1]->Sigla);
        if(num_baia1>0 && num_baia1<=TOTBAIE    ) strcpy(sigla, P.ba[num_baia1]->Sigla);
        if(AGV[NumeroTGV]->mission.Typ==MISS_CARICO_DA_MAGAZZINO    ) sprintf(stringa, MESS[556], sigla, num_mag, Forca);
        if(AGV[NumeroTGV]->mission.Typ==MISS_SCARICO_SU_MAGAZZINO   ) sprintf(stringa, MESS[557], sigla, num_mag, Forca);
     }
  }
  strcat(stringa, sigla_scaff);
  //-----------------------------------------------
  // Visualizzazione dati missione
  //-----------------------------------------------
  TextPresa->Caption    = Forca;
  TextMissioni->Caption = stringa;
  //-----------------------------------------------
  // MACCHINE
  // Sequenza di priorità chiamate
  //-----------------------------------------------
  memset(&OrdinaMac,   0, sizeof(OrdinaMac));
  memset(&PrioritaMac, 0, sizeof(PrioritaMac));
  //-----------------------------------------------
  // Calcolo dinamico priorità
  //-----------------------------------------------
  P.OrdineMac(&OrdinaMac[0]);
  for(i=0; i<MAXCHIAMATE; i++){
     PrioritaMac[i] = P.ch[OrdinaMac[i]]->Priorita;
  }
  //-----------------------------------------------
  // CLIENT
  // Lettura stato priorità da file recuperato da
  // Server
  //-----------------------------------------------
  if(IdPC>1){
     handle = sopen( "PRIORMAC.DAT", O_RDWR | O_BINARY, SH_DENYNO );
     if( handle<0 ) err=1;
     if(!err){
        len = read( handle, &PrioritaMac[0], sizeof( PrioritaMac ) );
        if (len != sizeof( PrioritaMac ))  err = 1;
     }
     close( handle );
  }
  //-----------------------------------------------------
  // Sul fronte di variazione di una qualunque priorità
  // aggiorno la lista cronologia di esecuzione missione
  //-----------------------------------------------------
  if(memcmp(&OldPrioritaMac[0], &PrioritaMac[0], sizeof(PrioritaMac))!=0){
     ListPrioritaMac->Clear();
     for(i=0; i<MAXCHIAMATE; i++){
        if(OrdinaMac[i]<1           ) continue;
        if(OrdinaMac[i]>MAXCHIAMATE ) continue;
        //------------------------------------------
        // Cerco se un TGV ha già preso la missione
        //------------------------------------------
        tgv_in_chiamata=0;
        for(j=1; j<=MAXAGV; j++){
           if(AGV[j]->mission.Chiamata==0) continue;
           //------------------------------------------
           // Scomposizione chiamata
           //------------------------------------------
           num_baia1=num_mac1=num_mag1=num_stz1=0;
           P.TestChiamataNum(AGV[j]->mission.Chiamata, &num_mag1, &num_baia1, &num_mac1, &num_stz1);
           if(num_mac1!=OrdinaMac[i]) continue;
           //------------------------------------------
           // TGV trovato
           //------------------------------------------
           tgv_in_chiamata = j;
           break;
        }
        sprintf(stringa, "%02d%10s (%5d) - %2d", OrdinaMac[i], P.ch[OrdinaMac[i]]->Sigla, P.ch[OrdinaMac[i]]->Priorita, tgv_in_chiamata);
        ListPrioritaMac->Items->Add(stringa);
     }
     memcpy(&OldPrioritaMac[0], &PrioritaMac[0], sizeof(OldPrioritaMac));
     //-----------------------------------------------
     // SERVER
     // Salvataggio su file per client l'ordine di
     // asservimento priorità
     //-----------------------------------------------
     if(IdPC<2){
        handle = sopen( "PRIORMAC.DAT", O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
        if( handle<0 ) err=1;
        if(!err){
           len= write( handle, &PrioritaMac[0], sizeof( PrioritaMac ));
           if (len != sizeof( PrioritaMac ))  err = 1;
        }
        close( handle );
     }
  }
  //-----------------------------------------------
  // BAIE
  // Sequenza di priorità chiamate
  //-----------------------------------------------
  memset(&OrdinaBaie,   0, sizeof(OrdinaBaie));
  memset(&PrioritaBaie, 0, sizeof(PrioritaBaie));
  //-----------------------------------------------
  // Calcolo dinamico priorità
  //-----------------------------------------------
  P.OrdineBaie(&OrdinaBaie[0]);
  for(i=0; i<MAXBAIE; i++){
     PrioritaBaie[i] = P.ba[OrdinaBaie[i]]->Priorita;
  }
  //-----------------------------------------------
  // CLIENT
  // Lettura stato priorità da file recuperato da
  // Server
  //-----------------------------------------------
  if(IdPC>1){
     handle = sopen( "PRIORBAI.DAT", O_RDWR | O_BINARY, SH_DENYNO );
     if( handle<0 ) err=1;
     if(!err){
        len = read( handle, &PrioritaBaie[0], sizeof( PrioritaBaie ) );
        if (len != sizeof( PrioritaBaie ))  err = 1;
     }
     close( handle );
  }
  //-----------------------------------------------------
  // Sul fronte di variazione di una qualunque priorità
  // aggiorno la lista cronologia di esecuzione missione
  //-----------------------------------------------------
  if(memcmp(&OldPrioritaBaie[0], &PrioritaBaie[0], sizeof(PrioritaBaie))!=0){
     ListPrioritaBaie->Clear();
     for(i=0; i<MAXBAIE; i++){
        if(OrdinaBaie[i]<1       ) continue;
        if(OrdinaBaie[i]>MAXBAIE ) continue;
        //------------------------------------------
        // Cerco se un TGV ha già preso la missione
        //------------------------------------------
        tgv_in_chiamata=0;
        for(j=1; j<=MAXAGV; j++){
           if(AGV[j]->mission.Chiamata==0) continue;
           //------------------------------------------
           // Scomposizione chiamata
           //------------------------------------------
           num_baia1=num_mac1=num_mag1=num_stz1=0;
           P.TestChiamataNum(AGV[j]->mission.Chiamata, &num_mag1, &num_baia1, &num_mac1, &num_stz1);
           if(num_baia1!=OrdinaBaie[i]) continue;
           //------------------------------------------
           // TGV trovato
           //------------------------------------------
           tgv_in_chiamata = j;
           break;
        }
        sprintf(stringa, "%04d%10s (%5d) - %2d", OrdinaBaie[i], P.ba[OrdinaBaie[i]]->Sigla, P.ba[OrdinaBaie[i]]->Priorita, tgv_in_chiamata);
        ListPrioritaBaie->Items->Add(stringa);
     }
     memcpy(&OldPrioritaBaie[0], &PrioritaBaie[0], sizeof(OldPrioritaBaie));
     //-----------------------------------------------
     // SERVER
     // Salvataggio su file per client l'ordine di
     // asservimento priorità
     //-----------------------------------------------
     if(IdPC<2){
        handle = sopen( "PRIORBAI.DAT", O_RDWR | O_BINARY | O_CREAT, SH_DENYNO, S_IREAD | S_IWRITE );
        if( handle<0 ) err=1;
        if(!err){
           len= write( handle, &PrioritaBaie[0], sizeof( PrioritaBaie ));
           if (len != sizeof( PrioritaBaie ))  err = 1;
        }
        close( handle );
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::BtnReset1Click(TObject *Sender)
{
  int NumTgv=0;
  bool ResetMissione;
  bool ResetChiamata;
  int  result, err;
  AnsiString InputStringa;
  char buffer[201]="";
  char NomeFile[100];

  //-----------------------------------------------------
  // Gestione Livello password
  //-----------------------------------------------------
  if(LivelloPassword<1) return;
  //------------------------------------------------
  // Numero del TGV selezionato
  //------------------------------------------------
  NumTgv=NumeroTGV;
  //------------------------------------------------
  // Richiesta di conferma annullamento missione
  //------------------------------------------------
  sprintf(buffer, MESS[341], NumTgv);
  result = Application->MessageBox(buffer, MESS[99], MB_YESNO | MB_ICONQUESTION);
  if(result != IDYES) return;
  //----------------------------------------------------------
  // Inizializzo reset missione
  //----------------------------------------------------------
  ResetMissione=true;
  ResetChiamata=true;
  //----------------------------------------------------------
  // Richiesta abilitazione reset chiamata
  //----------------------------------------------------------
  if(AGV[NumTgv]->mission.Chiamata!=0 && (AGV[NumTgv]->mission.PresCEST_A!=LIBERA || AGV[NumTgv]->mission.PresCEST_B!=LIBERA)){
     ResetChiamata=false;
     result = Application->MessageBox("Reset match the mission call?", MESS[99], MB_YESNO | MB_ICONQUESTION);
     if(result == IDYES) ResetChiamata=true;
  }
  //----------------------------------------------------------
  // Invio alla navette il comando di RESET MISSION (DM98=1)
  //----------------------------------------------------------
  if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
  //----------------------------------------------------------
  // SICUREZZA
  // Invio due volte il comando di RESET MISSIONE TGV
  //----------------------------------------------------------
  if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
  //----------------------------------------------------------
  // SICUREZZA 2
  // Invio 3 volte il comando di RESET MISSIONE TGV
  //----------------------------------------------------------
  if(ResetMissione==true && AGV[NumTgv]->trasmetti_reset_AGV(buffer)) ResetMissione=false;
  //----------------------------------------------------------
  // Segnalazione errore
  //----------------------------------------------------------
  if(ResetMissione==false){
     sprintf(buffer, "RESET NOT POSSIBLE: Comunication error with TGV%d!", NumTgv);
     ShowMessage(buffer);
     return;
  }
  //----------------------------------------------------------
  // Se sono verificate tutte le condizione necessarie per
  // il Reset delle Missioni al volo azzero anche la missione
  // in RAM.
  //----------------------------------------------------------
  if(ResetMissione==true){
     // --- Azzeramento dati missione ---
     AGV[NumTgv]->reset_mission(buffer);
     if(ResetChiamata==true) AGV[NumTgv]->reset_chiamata_AGV();
     AGV[NumTgv]->mission.TGVInCambioMissione=false;
     AGV[NumTgv]->mission.TimeInitAttesa=0;
  }

  //-----------------------------------------
  //  Trasmetti comando al SERVER
  //-----------------------------------------
  if( IdPC>1 ){
     sprintf( NomeFile, "ResetMissTgv%02d", NumTgv );
     err = RemCreaFileComando ( NomeFile, NULL, NULL, buffer );
     if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     //----------------------------------------------
     // Comando di reset chiamata
     //----------------------------------------------
     if(ResetChiamata==true){
        sprintf( NomeFile, "ResetChTgv%02d", NumTgv );
        err = RemCreaFileComando ( NomeFile, NULL, NULL, buffer );
        if( err ) Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }

  //-----------------------------
  // Traccia su LOG OPERATORE
  //-----------------------------
  sprintf(buffer, "(RES_MIS) - Reset missione Tgv%d", NumTgv);
  FileLogOperatore(buffer);
  if(ResetChiamata==true){
     sprintf(buffer, "(RES_CH ) - Reset chiamata Tgv%d", NumTgv);
     FileLogOperatore(buffer);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::UpDownScrollClick(TObject *Sender, TUDBtnType Button)
{
  int err;
  short int Idx;
  char Time[11]="";
  char Date[11]="";
  long int TimeRecord=0;
  char all_mess[101]="";

  Idx = (short int) (UpDownScroll->Position);
  //--------------------------------------------------
  // TELECAMERINA
  //--------------------------------------------------
  if(LogTgvAttivato==true){
     //--------------------------------------------------
     // Recupero della situazione del TGV
     //--------------------------------------------------
     err = FormMain->LoadLogTgv(Idx, &TimeRecord, all_mess);
     if(err ){
        LabelScroll2->Color   = clRed;
        sprintf(all_mess, "Indice: %d - STOP!", Idx);
        LabelScroll2->Caption = all_mess;
     }
     if(!err){
        LabelScroll2->Color   = clLime;
        sprintf(all_mess, "Indice: %d - IN CORSO...", Idx);
        LabelScroll2->Caption = all_mess;
     }
     //-----------------------------------------
     // Aggiornamento della Form principale con
     // Data e Ora del record.
     //-----------------------------------------
     timeint_to_timechar(&TimeRecord, &Date[0], &Time[0]);
     sprintf(all_mess, "%s - %s", Date, Time);
     LabelScroll1->Caption = all_mess;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormStato::LabelScroll2Click(TObject *Sender)
{
  int err;
  short int Idx;
  long int TimeRecord;
  char Date[11]="";
  char Time[11]="";
  char all_mess[101]="";
  AnsiString InputStringa;

  Idx=1;
  //--------------------------------------------------
  // TELECAMERINA
  //--------------------------------------------------
  if(LogTgvAttivato==true){
     //-----------------------------------------------
     // Richiesta "Indice" del Record Desiderato
     //-----------------------------------------------
     InputStringa = InputBox("LOG TGV", "Introdurre l'indice di posizionamento:", Idx);
     if(InputStringa.IsEmpty() == true) return;
     Idx = (short int)(StrToInt(InputStringa));
     if(Idx<1) Idx=1;
     //-----------------------------------------------
     // Recupero della situazione del TGV
     //-----------------------------------------------
     err = FormMain->LoadLogTgv(Idx, &TimeRecord, all_mess);
     if(err ){
        LabelScroll2->Color   = clRed;
        sprintf(all_mess, "Indice: %d - STOP!", Idx);
        LabelScroll2->Caption = all_mess;
     }
     if(!err){
        LabelScroll2->Color   = clLime;
        sprintf(all_mess, "Indice: %d - IN CORSO...", Idx);
        LabelScroll2->Caption = all_mess;
     }
     //-----------------------------------------------
     // Aggiornamento della Form principale con Data
     // e Ora del record.
     //-----------------------------------------------
     timeint_to_timechar(&TimeRecord, &Date[0], &Time[0]);
     sprintf(all_mess, "%s - %s", Date, Time);
     LabelScroll1->Caption = all_mess;
     UpDownScroll->Position = Idx;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormStato::LabGrdDblClick(TObject *Sender)
{
  //---------------------------------
  // Abilitazione veloce password
  //---------------------------------
  if(AttivaPassword3Veloce){
     if(LivelloPassword<LivelloMaxPassword){
        LivelloPassword=LivelloMaxPassword;
     }else LivelloPassword=LivelloMinPassword;
     FormMain->StatoPassword();
  }
}
//---------------------------------------------------------------------------


void __fastcall TFormStato::TextChiamataDblClick(TObject *Sender)
{
  char buffer[101]="";
  AnsiString InputStringa;

  //------------------------------------
  // Password non ammessa
  //------------------------------------
  if(LivelloPassword<3     ) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  sprintf(buffer, "%06d", AGV[NumeroTGV]->mission.Chiamata);
  InputStringa = InputBox("Chiamata", "Forzatura chiamata:", buffer);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  try{
     AGV[NumeroTGV]->mission.Chiamata = StrToInt(InputStringa);
  }
  catch(...){
     ShowMessage("Dato non ammesso...");
     return;
  }
  //-----------------------------------------------
  // Traccia su log operatore
  //-----------------------------------------------
  sprintf(buffer, "(FORZTGV) - Forzato chiamata su TGV%d", NumeroTGV);
  FileLogOperatore(buffer);
}
//---------------------------------------------------------------------------

void __fastcall TFormStato::LabGLaserDblClick(TObject *Sender)
{
  char buffer[101]="";
  AnsiString InputStringa;

  //------------------------------------
  // Password non ammessa
  //------------------------------------
  if(LivelloPassword<3     ) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox("G LASER", "Forzatura G:", "0");
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  try{
     AGV[NumeroTGV]->stato.g_laser = StrToInt(InputStringa);
  }
  catch(...){
     ShowMessage("Dato non ammesso...");
     return;
  }
  //-----------------------------------------------
  // Traccia su log operatore
  //-----------------------------------------------
  sprintf(buffer, "(FORZTGV) - Forzato g_laser su TGV%d", NumeroTGV);
  FileLogOperatore(buffer);
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::TextBatteriaDblClick(TObject *Sender)
{
  char buffer[101]="";
  AnsiString InputStringa;

  //------------------------------------
  // Password non ammessa
  //------------------------------------
  if(LivelloPassword<3     ) return;
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  sprintf(buffer, "%d", AGV[NumeroTGV]->mission.PercCaricaBatt);
  InputStringa = InputBox("Chiamata", "Forzatura % batteria:", buffer);
  if(InputStringa.IsEmpty() == true) return;
  //-----------------------------------------------
  // Verifico valore rotazione
  //-----------------------------------------------
  try{
     AGV[NumeroTGV]->mission.PercCaricaBatt = (BYTE)(StrToInt(InputStringa));
     if(AGV[NumeroTGV]->mission.PercCaricaBatt>100) AGV[NumeroTGV]->mission.PercCaricaBatt=100;
  }
  catch(...){
     ShowMessage("Dato non ammesso...");
     return;
  }
  //-----------------------------------------------
  // Traccia su log operatore
  //-----------------------------------------------
  sprintf(buffer, "(FORZTGV) - Forzato percentuale batteria su TGV%d", NumeroTGV);
  FileLogOperatore(buffer);
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  Screen->Cursor=crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::btnLogViewClick(TObject *Sender)
{
  int  i;
  char NomeFile[201]="LogMovim.txt";
  char NomeFile2[2001]="LogMovim.txt";

  //------------------------------------------------------------
  //  Il Client recupera file da SERVER
  //------------------------------------------------------------
  if( IdPC>1 ){
    sprintf( NomeFile2, "%s\\%s", DriveServer, NomeFile );
    CopyFile( NomeFile2, NomeFile, FALSE );
  }
  //------------------------------------------------------------
  // se il file non esiste ERRORE
  //------------------------------------------------------------
  if(!FileExists(NomeFile)){
    Application->MessageBox("File not exist!", MESS[100], MB_OK | MB_ICONEXCLAMATION);
    return;
  }
  //------------------------------------------------------------
  strcpy(FormListagen->NomeFileLista, NomeFile);
  strcpy(FormListagen->CaptionFileLista, "Log Movimenti");
  FormListagen->CarSeparatore=';'; // Carattere separatore campi nel file
  //-------------------------------------------------------------------------------------
  // Dimensioni delle colonne ed Intestazioni delle colonne
  // sarebbe possibile impostarle a -1 per fare l'autosize,
  // ma poi la funzione di stampa non formatta bene il testo!
  //-------------------------------------------------------------------------------------
  memset(&FormListagen->CaptionColumn,0,sizeof(FormListagen->CaptionColumn));

  strcpy(FormListagen->CaptionColumn[ 0], "DateTime");     FormListagen->ColSize[ 0]=125; // Data &  ora
  strcpy(FormListagen->CaptionColumn[ 1], "Operation");    FormListagen->ColSize[ 1]=160; // Operazione
  strcpy(FormListagen->CaptionColumn[ 2], "TGV");          FormListagen->ColSize[ 2]= 40; // AGV
  strcpy(FormListagen->CaptionColumn[ 3], "Position");     FormListagen->ColSize[ 3]=150; // Posizione
  strcpy(FormListagen->CaptionColumn[ 4], "Code");         FormListagen->ColSize[ 4]= 90;
  strcpy(FormListagen->CaptionColumn[ 5], "Lote");         FormListagen->ColSize[ 5]= 90;
  strcpy(FormListagen->CaptionColumn[ 6], "BoxType");      FormListagen->ColSize[ 6]= 90;
  strcpy(FormListagen->CaptionColumn[ 7], "Carwash");      FormListagen->ColSize[ 7]= 40;
  strcpy(FormListagen->CaptionColumn[ 8], "Low");          FormListagen->ColSize[ 8]= 40;

  for(i=0; i<MAXUDCBOX; i++){
     sprintf(FormListagen->CaptionColumn[ 9+(i*5)], "%d.HU", i+1);       FormListagen->ColSize[ 9+(i*5)]=100;
     sprintf(FormListagen->CaptionColumn[10+(i*5)], "%d.Batch", i+1);    FormListagen->ColSize[10+(i*5)]= 80;
     sprintf(FormListagen->CaptionColumn[11+(i*5)], "%d.Shift", i+1);    FormListagen->ColSize[11+(i*5)]= 80;
     sprintf(FormListagen->CaptionColumn[12+(i*5)], "%d.Status", i+1);   FormListagen->ColSize[12+(i*5)]= 50;
     sprintf(FormListagen->CaptionColumn[13+(i*5)], "%d.Location", i+1); FormListagen->ColSize[13+(i*5)]= 80;
  }
  FormListagen->ColNum = 9+5*(MAXUDCBOX);         // numero di colonne totali
  //-------------------------------------------------------------------------------------
  FormListagen->TipoFileLista=5;
  //FormListagen->reverseFile=false;

  FormListagen->btnExportExcel->Enabled  =true;
  FormListagen->btnSaveAsCSV->Enabled    =true;
  if(LivelloPassword<=2) FormListagen->btnAzzera->Enabled=false;
  else FormListagen->btnAzzera->Enabled=true;

  FormListagen->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::btnSearchUDCClick(TObject *Sender)
{
  int CentroX, CentroY;
  char udc[21]="";
  char buffer[101]="";
  bool Trovato=false;
  short int z;
  short int NumMag,Pos,Pin;
  short int Punto;
  AnsiString InputStringa="";
  //-----------------------------------------------
  // Richiesta "Chiave" del Record Desiderato
  //-----------------------------------------------
  InputStringa = InputBox("Search UDC", "Input UDC number to search:", "");
  if(InputStringa.IsEmpty()==true) return;

  strcpy(udc, InputStringa.c_str());
  if(strcmp(udc,"")==0) return;
  //-------------------------------------------
  // Ricerca tra tutti i magazzini in elenco
  //-------------------------------------------
  for(NumMag=1; NumMag<=MAXMAG; NumMag++){
     //------------------------------------------
     // Cerca Magazzino compatibile
     //------------------------------------------
     if(M.m[NumMag]->Num<1              ) continue;
     if(M.m[NumMag]->Num>TOTMAG         ) continue;
     if(M.m[NumMag]->Npt==0             ) continue;
     if(M.m[NumMag]->PuntoIng==0        ) continue;
     if(M.m[NumMag]->PuntoIng>TOTPUNTI  ) continue;
     if(M.m[NumMag]->PuntoOut==0        ) continue;
     if(M.m[NumMag]->PuntoOut>TOTPUNTI  ) continue;

     for(Pos=1; Pos<=MAXTERRAMAG; Pos++){
        for(Pin=MAXPIANIMAG-1; Pin>=0; Pin--){
           if(M.m[NumMag]->Pos[Pos].Presenza[Pin]==false) continue;
           for(z=0; z<MAXUDCBOX; z++){
              if(strcmp(M.m[NumMag]->Pos[Pos].Box[Pin].Prd[z].Udc,udc)==0){
                 Punto=M.m[NumMag]->Pos[Pos].NodoIng;
                 //----------------------------------------------
                 // Controllo della validità del punto trovato
                 //----------------------------------------------
                 if(Punto<1 || Punto>MAXPUNTI){
                    Application->MessageBox("Wrong UDC Location","Error", MB_OK | MB_ICONEXCLAMATION);
                    return;
                 }
                 //-----------------------------------------------
                 // Posizionamento su nodo richiesto
                 //-----------------------------------------------
                 CentroX = (int)(FormLayout->SfondoW/2);
                 CentroY = (int)(FormLayout->SfondoH/2);
                 //--------------------------------------------------------
                 // Se è stato punto un NODO centro il layout su quel nodo
                 //--------------------------------------------------------
                 offset[1][0]=Punti[Punto][0]-((CentroX*Rap)/Rid);
                 offset[1][1]=Punti[Punto][1]-((CentroY*Rap)/Rid);

                 FormLayout->PosizionamentoSuNodo=(short int)(Punto);
                 Trovato=true;
                 //FormLayout->PosizLocaz=(short int)(Locazione);
              }
           }
        }
     }
  }
  if(Trovato==false){
     sprintf(buffer, "Code '%s' Not Found",udc);
     Application->MessageBox(buffer, "Warning", MB_OK + MB_ICONWARNING + MB_TOPMOST);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormStato::btnAttivaFunzSpecClick(TObject *Sender)
{
  if(toggle==true){
     toggle=false;
     Panel2->Top=595;
     btnAttivaFunzSpec->Glyph=btnGliphDown->Glyph;
  }
  else{
     toggle=true;
     Panel2->Top=700;
     btnAttivaFunzSpec->Glyph=btnGliphUp->Glyph;
  }
}
//---------------------------------------------------------------------------


