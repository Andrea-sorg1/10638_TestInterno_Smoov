//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>

#pragma hdrstop

#include "TestP.h"
#include "Tecnofer.h"

//---------------------------------------------------------------------------
//----------------------------------------------------
// Variabili definite in GRAPHVAR.CPP
//----------------------------------------------------
extern int offset[][2];

//  ---------------------
//   STRUTTURA NODI
//  ---------------------
//  0,1 = x,y posizione centrale nodo
//  2 = numero nodo
//  3 = occupazione agv ( numero AGV oppure libero = 0 )
//-------------------------------------------------------
extern int Punti[][5];

//  ---------------------
//   STRUTTURA PERCORSO
//  ---------------------
//  0 = Numero nodo partenza
//  1 = Numero nodo arrivo
//-------------------------------------------------------
extern int Percorso[][2];
extern int PercorsoCurva[][5];
extern int PercorsoCurvaBezier[][7];

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTest *FormTest;
//---------------------------------------------------------------------------
__fastcall TFormTest::TFormTest(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::FormActivate(TObject *Sender)
{
  RGAttivaTest->Checked=true;
  SendMessage(ProgressBar->Handle, PBM_SETBARCOLOR, 0,RGB(60,120,180));
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::RGAttivaTestClick(TObject *Sender)
{
  TestBox->Enabled=true;
  CompilaBox->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::RGAttivaCompilaClick(TObject *Sender)
{
  TestBox->Enabled=false;
  CompilaBox->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::BtnTestClick(TObject *Sender)
{
  char Appoggio[5]="";
  short int Inizio=0;

  //-----------------------------------
  // Recupero nodo di start
  //-----------------------------------
  Inizio = (short int)(atoi(EditInizio->Text.c_str()));
  if(Inizio<=0 || Inizio>=TOTPUNTI){
     Inizio=1;
  }
  sprintf(Appoggio, "%04d", Inizio);
  EditInizio->Text=Appoggio;

  //-----------------------------------
  // Recupero test speciali
  //-----------------------------------
  AGV[1]->PercorsiSpeciali = (short int)(atoi(EditTestSpeciali->Text.c_str()));
  sprintf(Appoggio, "%01d", AGV[1]->PercorsiSpeciali);
  EditTestSpeciali->Text=Appoggio;
  //-----------------------------------
  // Inizio test percorsi
  //-----------------------------------
  Label->Visible=true;
  Screen->Cursor=crHourGlass;

  Refresh();
  Test( Sender, Inizio );

  Screen->Cursor=crDefault;
  Label->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){ case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------
void TFormTest::Test(TObject *Sender, short int Inizio)
{
  char buffer[201]="";
  char buffer1[201]="";
  int  i, j, match, num_agv;
  int  n_rec, err, conta_err, Zp;
  short int tipo_path;
  short int perc[MAXPERCORSI];
  struct DATI_PERCORSO dati[MAXPERCORSI];
  short int perc1[MAXPERCORSI];
  struct DATI_PERCORSO dati1[MAXPERCORSI];
  short int perc2[MAXPERCORSI];
  struct DATI_PERCORSO dati2[MAXPERCORSI];
  short int sorg, dest;
  int lista_errori[MAXPUNTI+1][2];
  char str_errori[TOTPUNTI+1][101];
  struct NOD nod;
  bool sorg_fuori_ing;
  bool dest_fuori_ing;
  bool err_trovato;
  long int TimeInizio, TimeFine, TimeDiff;
  int EscStatus=0;

  ProgressBar->Position=0;

  //------------------------------------------------------------------
  // 30/10/2014 Esclusione ZP selezionate dal TEST ( per velocizzare )
  //-------------------------------------------------------------------
  TestEsclZp[ 0]=TestEsclZp0->Checked;
  TestEsclZp[ 1]=TestEsclZp1->Checked;
  TestEsclZp[ 2]=TestEsclZp2->Checked;
  TestEsclZp[ 3]=TestEsclZp3->Checked;
  TestEsclZp[ 4]=TestEsclZp4->Checked;
  TestEsclZp[ 5]=TestEsclZp5->Checked;
  TestEsclZp[ 6]=TestEsclZp6->Checked;
  TestEsclZp[ 7]=TestEsclZp7->Checked;
  TestEsclZp[ 8]=TestEsclZp8->Checked;
  TestEsclZp[ 9]=TestEsclZp9->Checked;
  TestEsclZp[10]=TestEsclZp10->Checked;
  TestEsclZp[11]=TestEsclZp11->Checked;
  TestEsclZp[12]=TestEsclZp12->Checked;
  TestEsclZp[13]=TestEsclZp13->Checked;
  TestEsclZp[14]=TestEsclZp14->Checked;
  TestEsclZp[15]=TestEsclZp15->Checked;
  TestEsclZp[16]=TestEsclZp16->Checked;
  TestEsclZp[17]=TestEsclZp17->Checked;
  TestEsclZp[18]=TestEsclZp18->Checked;
  TestEsclZp[19]=TestEsclZp19->Checked;

  num_agv=atoi(EditAgv->Text.c_str());
  if(num_agv<1 || num_agv>MAXAGV){
     TestListBox->Items->Add("Errore: AGV non valido!");
     return;
  }
  conta_err=0;
  memset(&nod, 0, sizeof(NOD));
  tipo_path = RGTipo->ItemIndex;
  TimeInizio=time(NULL);
  //------------------------------------
  // Prima Parte
  //------------------------------------
  TestListBox->Items->Clear();
  TestListBox->ItemIndex=0;
  memset(&lista_errori, 0, sizeof(lista_errori));
  if(AGV[num_agv]->test_mission_on()){
     TestListBox->Items->Add("Errore: Per eseguire il test l'AGV non deve avere missioni in corso!");
     return;
  }

  for( n_rec=1; n_rec<=MAXPUNTI; n_rec++){
     if( N.n[ n_rec ]->num >0 ) break;
  }
  if( n_rec>MAXPUNTI ) return;
  N.cerca_record(&nod, n_rec );
  //------------------------------------------------------
  // GESTIONE LASER
  // Nel caso dei TGV laser non è necessario che 2 nodi
  // siano collegati dalla struttura percorsi per essere
  // percorribili (manca la scheda di lettura traccia)
  //------------------------------------------------------
  if(!AGV[num_agv]->mission.GestioneLaser){
     ProgressBar->Max=MAXPUNTI-1;
     while(n_rec<=MAXPUNTI){
        //------------------------------------------------------
        ProgressBar->Position=n_rec;
        //------------------------------------------------------
        for(i=0;i<MAXPUNTINODO; i++){
           //------------------------------------------------------------------
           // 30/10/2014 Escludi le ZP selezionate dal TEST ( per velocizzare )
           //-------------------------------------------------------------------
           Zp=N.n[n_rec]->zp;
           if(TestEsclZp[Zp]==true) continue;

           sprintf(buffer, "Test punto sucessivo --> Nodo: %03d", nod.num);
           if(nod.rec[i].ps){
              match=0;
              j=0;
              while(Percorso[j][0]){
                 if(nod.num==Percorso[j][0]){
                    if(nod.rec[i].ps==Percorso[j][1]){
                       match=1;
                       break;
                    }
                 }
                 else{
                    if(nod.num==Percorso[j][1]){
                       if(nod.rec[i].ps==Percorso[j][0]){
                          match=1;
                          break;
                       }
                    }
                 }
                 j++;
              }
              if(!match && (conta_err < MAXPUNTI-1)){
                 lista_errori[conta_err][0]=nod.num;
                 lista_errori[conta_err][1]=i;
                 conta_err++;
              }
           }
        }
        for( n_rec=n_rec+1; n_rec<=MAXPUNTI; n_rec++){
           if( N.n[ n_rec ]->num >0 ) break;
        }
        N.cerca_record(&nod, n_rec );
     }
     if(conta_err){
        for(j=0;j<=MAXPUNTI;j++){
           if(lista_errori[j][0]==0) continue;
           sprintf(buffer, "Errore: Nodo: %03d  ---  Riga: %03d", lista_errori[j][0], lista_errori[j][1]);
           TestListBox->Items->Add(buffer);
        }
     }
     sprintf( buffer, "Test concluso con %04d Errori!", conta_err);
     TestListBox->Items->Add(buffer);
     TestListBox->Items->Add("-----------------------------------------------------------------------------------------------------------------------------------------");
  }
  //------------------------------------
  // Seconda Parte
  //------------------------------------
  sorg      =Inizio;
  conta_err =0;
  memset(&str_errori, 0, sizeof(str_errori));
  ProgressBar->Max=MAXPUNTI-1;
  for(; sorg<TOTPUNTI; sorg++){
     if(N.n[sorg]->num<1        ) continue;
     if(N.n[sorg]->num>TOTPUNTI ) continue;
     if(conta_err>50) break;
     //------------------------------------------------------------------
     ProgressBar->Position=sorg;

     EscStatus=GetAsyncKeyState(VK_CONTROL);
     if(EscStatus<0){
        MessageDlg("Stopped",  mtError, TMsgDlgButtons() << mbOK, 0);
        ProgressBar->Position=0;
        break;
     }

     //------------------------------------------------------------------
     // 30/10/2014 Escludi le ZP selezionate dal TEST ( per velocizzare )
     //-------------------------------------------------------------------
     Zp=N.n[sorg]->zp;
     if(TestEsclZp[Zp]==true) continue;
     //---------------------------------------------------------------
     // 14/07/2008 Escludi i NODI SLAVE dal TEST ( per velocizzare )
     //---------------------------------------------------------------
     if( TestEsclusioneNodiSlave->Checked == true ){
        if( N.n[ sorg ]->NodoMaster >0 ) continue; // ->SKIP
     }
     // -- verifico se il sorgente è nodo fuori ingombro
     sorg_fuori_ing=false;
     for(j=0; j<MAXNODIATTESA; j++){
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1         ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>MAXPUNTI  ) continue;
        if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=sorg     ) continue;
        sorg_fuori_ing=true;
        break;
     }
     // -- solo se il sorgente è nodo fuori ingombro o PT
     if(TestEsclusioneStartNodiPerc->Checked==false || (N.punti_notevoli[sorg]>0 || sorg_fuori_ing==true)){
        for(dest=1; dest<MAXPUNTI; dest++){
           if(N.n[dest]->num<1        ) continue;
           if(N.n[dest]->num>TOTPUNTI ) continue;
           //------------------------------------------------------------------
           // 30/10/2014 Escludi le ZP selezionate dal TEST ( per velocizzare )
           //-------------------------------------------------------------------
           Zp=N.n[ dest ]->zp;
           if(TestEsclZp[Zp]==true) continue;
           //---------------------------------------------------------------
           // 14/07/2008 Escludi i NODI SLAVE dal TEST ( per velocizzare )
           //---------------------------------------------------------------
           if( TestEsclusioneNodiSlave->Checked == true ){
              if( N.n[ dest ]->NodoMaster >0 ) continue; // ->SKIP
           }
           // -- verifico se il destinazione è nodo fuori ingombro
           dest_fuori_ing=false;
           for(j=0; j<MAXNODIATTESA; j++){
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]<1         ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]>MAXPUNTI  ) continue;
              if(N.nodi_attesa[GRUPPO_FUORI_INGOMBRO][j]!=dest     ) continue;
              dest_fuori_ing=true;
              break;
           }
           // -- solo se il destinazione è nodo fuori ingombro o PT
           sprintf(buffer, "Test percorso: Sorgente: %03d  ---  Destinazione: %03d", sorg, dest);
           if((N.punti_notevoli[dest]>0 || dest_fuori_ing==true) && (sorg!=dest)){
              //-------------------------------------------------------------
              // PERCORSO DIRETTO
              // Verifica il percorso originale del TGV da START a DEST.
              //-------------------------------------------------------------
              memset(&perc[0], 0, sizeof(perc));
              memset(&dati[0], 0, sizeof(dati));
              memset(&perc1[0], 0, sizeof(perc1));
              memset(&dati1[0], 0, sizeof(dati1));
              memset(&perc2[0], 0, sizeof(perc2));
              memset(&dati2[0], 0, sizeof(dati2));
              err = N.cerca_path(num_agv, tipo_path, sorg, dest, perc, dati, buffer);
              if(err!=0){
                 err_trovato=false;
                 //-----------------------------------------
                 // Verifico se errore già scritto in lista
                 //-----------------------------------------
                 for(j=0; j<TOTPUNTI; j++){
                    if(strcmp(str_errori[j], "")==0    ) break;
                    if(strcmp(str_errori[j], buffer)!=0) continue;
                    err_trovato=true;
                    break;
                 }
                 if(err_trovato==true) continue;  // errore già conteggiato --> continue;
                 //-----------------------------------------
                 // Inserimento nuovo errore in lista
                 //-----------------------------------------
                 if(err_trovato==false){
                    //---------------------------------------
                    // Visualizzazione in finestra
                    //---------------------------------------
                    conta_err++;
                    if(conta_err<100) TestListBox->Items->Add(buffer);
                    //---------------------------------------
                    // Archiviazione tra errori già scritti
                    //---------------------------------------
                    for(j=0; j<TOTPUNTI; j++){
                       if(strcmp(str_errori[j], "")!=0) continue;
                       strcpy(str_errori[j], buffer);
                       break;
                    }
                    continue;
                 }
              }
              //-------------------------------------------------------------
              // GESTIONE CURVE
              // Verifico l'esistenza e la percorribiltà della curva.
              //-------------------------------------------------------------
              err = P.ImpostazioneCurve(perc, dati, perc1, dati1, buffer1);
              if(err!=0){
                 err_trovato=false;
                 sprintf(buffer, "%d --> %d %s", sorg, dest, buffer1);
                 //-----------------------------------------
                 // Verifico se errore già scritto in lista
                 //-----------------------------------------
                 for(j=0; j<TOTPUNTI; j++){
                    if(strcmp(str_errori[j], "")==0    ) break;
                    if(strcmp(str_errori[j], buffer)!=0) continue;
                    err_trovato=true;
                    break;
                 }
                 if(err_trovato==true) continue;  // errore già conteggiato --> continue;
                 //-----------------------------------------
                 // Inserimento nuovo errore in lista
                 //-----------------------------------------
                 if(err_trovato==false){
                    //---------------------------------------
                    // Visualizzazione in finestra
                    //---------------------------------------
                    conta_err++;
                    if(conta_err<100) TestListBox->Items->Add(buffer);
                    //---------------------------------------
                    // Archiviazione tra errori già scritti
                    //---------------------------------------
                    for(j=0; j<TOTPUNTI; j++){
                       if(strcmp(str_errori[j], "")!=0) continue;
                       strcpy(str_errori[j], buffer);
                       break;
                    }
                    continue;
                 }
              }
              //-------------------------------------------------------------
              // GESTIONE DEVIAZIONI
              // Verifico l'esistenza e la percorribiltà della curva.
              //-------------------------------------------------------------
              err = P.ImpostazioneDeviazioni(num_agv, perc, dati, perc2, dati2, buffer1);
              if(err!=0){
                 err_trovato=false;
                 sprintf(buffer, "%d --> %d %s", sorg, dest, buffer1);
                 //-----------------------------------------
                 // Verifico se errore già scritto in lista
                 //-----------------------------------------
                 for(j=0; j<TOTPUNTI; j++){
                    if(strcmp(str_errori[j], "")==0    ) break;
                    if(strcmp(str_errori[j], buffer)!=0) continue;
                    err_trovato=true;
                    break;
                 }
                 if(err_trovato==true) continue;  // errore già conteggiato --> continue;
                 //-----------------------------------------
                 // Inserimento nuovo errore in lista
                 //-----------------------------------------
                 if(err_trovato==false){
                    //---------------------------------------
                    // Visualizzazione in finestra
                    //---------------------------------------
                    conta_err++;
                    if(conta_err<100) TestListBox->Items->Add(buffer);
                    //---------------------------------------
                    // Archiviazione tra errori già scritti
                    //---------------------------------------
                    for(j=0; j<TOTPUNTI; j++){
                       if(strcmp(str_errori[j], "")!=0) continue;
                       strcpy(str_errori[j], buffer);
                       break;
                    }
                    continue;
                 }
              }
           }
        }
     }
  }
  //------------------------------------------------------------------
  // 30/10/2014 Stampa tempo trascorso
  //-------------------------------------------------------------------
  TimeFine=time(NULL);
  TimeDiff=TimeFine-TimeInizio;
  sprintf( buffer, "Tempo trascorso %d (sec)     ", TimeDiff);
  TestListBox->Items->Add("-----------------------------------------------------------------------------------------------------------------------------------------");
  TestListBox->Items->Add(buffer);

  sprintf( buffer, "Test concluso con %d Errori! in %d sec.", conta_err, TimeDiff);
  Application->MessageBox(buffer, "", MB_OK | MB_ICONINFORMATION);
  AGV[num_agv]->reset_ram_mission();
  return;
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::BtnCompMastSlaveClick(TObject *Sender)
{
  AnsiString InputStringa;
  char  buffer[201]="";
  int   result;
  int   err=0;
  int   NodoMaster, NodoSlave;
  int   CtrSlave;
  int   ArraySlave[ 50];
  int   TotSlave;
  int   i=0;
  int   Slave;
  int   Linea;
  int   prec, succ;
  int   x1, x2, y1, y2, distX, distY;
  int   Id, Raggio, Circonferenza;
  short int p1, p2;
  double dist;

  //---------------------------------------
  // RICHIESTA CONFERMA
  //---------------------------------------
  if(Application->MessageBox("Questa procedura compilerà i nodi SLAVE con uscita sul percorso\nnel caso che la destinazione sia un altro nodo della stessa fila...\nContinua?", "USCITA NODI SLAVE", MB_YESNO | MB_ICONQUESTION)!=IDYES) return;

  for(NodoMaster=1; NodoMaster<=TOTPUNTI; NodoMaster++){
     if(N.punti_notevoli[NodoMaster]==false) continue;

     memset( ArraySlave, 0, sizeof(ArraySlave));
     //--------------------------------------------------
     // Compila Array Slave appartenenti a questo MASTER
     //--------------------------------------------------
     CtrSlave=1;
     for(i=0; i<MAXPUNTINODO-1; i++){
        if( N.n[NodoMaster]->rec[i].pt==PERC_ALTERNATIVO_VELOCE ) continue;
        if( N.n[NodoMaster]->rec[i].pt && N.n[NodoMaster]->rec[i].pt!=NodoMaster ){

           NodoSlave = N.n[NodoMaster]->rec[i].pt;
           if(NodoSlave<1                            ) continue;
           if(NodoSlave>TOTPUNTI                     ) continue;
           if(N.punti_notevoli[NodoSlave]==false     ) continue;
           if(N.n[NodoSlave]->NodoMaster!=NodoMaster ) continue;


           ArraySlave[ CtrSlave ]=N.n[NodoMaster]->rec[i].pt;
           CtrSlave++;
        }
     }
     ArraySlave[0] = NodoMaster;
     TotSlave=CtrSlave-1;

     if(TotSlave==0) continue;

     //--------------------------------------------------
     // Compila Record SLAVE
     //--------------------------------------------------
     for( CtrSlave=1; CtrSlave<=TotSlave; CtrSlave++){
        //-----------------------------------------------
        //  Verifica se record esistente
        //-----------------------------------------------
        Slave = ArraySlave[ CtrSlave ];

        Linea=1;
        for( i=0; i<=TotSlave; i++){
           //-----------------------------------------------
           // Con destinazione se stesso --> SKIP
           //-----------------------------------------------
           if( i==CtrSlave ) continue;

           N.n[ Slave ]->rec[ Linea ].pt = (WORD)  ArraySlave[ i ];

           //-----------------------------------------------
           // In caso si scaffali mi devo assicurare di
           // tornare sempre sulla traccia per raggingere
           // per posizioni precedenti
           //-----------------------------------------------
           if( i<CtrSlave ){
              N.n[ Slave ]->rec[ Linea ].ps = N.n[ NodoMaster ]->rec[0].ps;
           }

           //-----------------------------------------------
           // Con destinazione Sucessiva
           //-----------------------------------------------
           if( i>CtrSlave ) N.n[ Slave ]->rec[ Linea ].ps = (WORD)  ArraySlave[ CtrSlave+1];
           Linea++;
        }


        //--------------------------------------------
        // RICALCOLO DISTANZE NODI
        // scorrimento di tutte le righe di percorso
        //--------------------------------------------
        p1 = N.n[Slave]->num;
        for(i=0; i<MAXPUNTINODO; i++){
           if(N.n[Slave]->rec[i].ps==0) continue;
           dist = 0;
           p2 = N.n[Slave]->rec[i].ps;
           //----------------------------------
           // Recupero coordinate
           //----------------------------------
           x1 = Punti[p1][0];
           y1 = Punti[p1][1];
           x2 = Punti[p2][0];
           y2 = Punti[p2][1];
           //----------------------------------
           // Se i nodi si trovano in retta la
           // distanza viene calcolata con il
           // teorema di pitagora.
           //----------------------------------
           distX = abs(x1-x2);
           distY = abs(y1-y2);
           if(distX>30000 || distY>30000) continue;
           //----------------------------------
           // Calcolo della distanza
           //----------------------------------
           dist = (double)(sqrt_algo((distX*distX)+(distY*distY)));
           //----------------------------------
           // Calcolo della distanza percorsa
           // su un arco di circonferneza.
           //----------------------------------
           Id=Raggio=Circonferenza=0;
           if(!N.CercaCurva(TRUE, p1, p2, &Raggio, &Circonferenza, &Id)){
              dist = (short int)(Circonferenza);
           }
           N.n[Slave]->rec[i].dist=(short int)(dist);
        }
        //-----------------------------------------
        //  Salva record sul file
        //-----------------------------------------
        err = N.save_record_nodi( Slave, buffer);
        if( err ) Application->MessageBox(buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
     }
  }
  ShowMessage("...END!!!");
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::BtnCompSpecClick(TObject *Sender)
{
//---------------------------------------
  // RICHIESTA CONFERMA
  //---------------------------------------
  if(Application->MessageBox("Are you sure?", "SPECIAL COMPILE", MB_YESNO | MB_ICONQUESTION)!=IDYES) return;
/*
  int stato = 0, j=0;
  int i=0;
  int num0, num1, num2;
  int n_rec, err, k;
  char buffer[201]="";
  struct NOD nod;
  short int ps9, av9, ind9, rot9, ral9, dist9;

  //---------------------------------------
  //  cerca il primo record
  //---------------------------------------
  for( n_rec=1; n_rec<=MAXPUNTI; n_rec++){
     if( N.n[ n_rec ]->num >0 ) break;
  }
  if( n_rec>MAXPUNTI ) return;


  for(;;){
     memset(&nod, 0, sizeof(NOD));
     stato = N.cerca_record(&nod, n_rec );
     if( stato ) {
        sprintf(buffer, "Nodo %d inesistente...", n_rec);
        FileTraccia(buffer);
        return;
     }
     num0=nod.num;

//   for( i=0;i<=MAXPUNTINODO-1;i++){
//      if(nod.rec[i].ps==0  ) break;
//      if(nod.rec[i].pt!=327) continue;

//      sprintf(buffer, "Nodo:%d", num0);
//      FileTraccia(buffer);

//      break;
//   }


//   for( i=0;i<=MAXPUNTINODO-1;i++){
//      if(nod.rec[i].ps==0  ) break;
//      if(nod.rec[i].ral>2  ) continue;
//
//      nod.rec[i].ral=9;
//   }


//   ps9=av9=ind9=rot9=ral9=dist9=0;
//   for( i=0;i<=MAXPUNTINODO-1;i++){
//      if(nod.rec[i].ps==0   ) break;
//      if(nod.rec[i].pt!=121) continue;
//      ps9   = nod.rec[i].ps;
//      av9   = nod.rec[i].av;
//      ind9  = nod.rec[i].ind;
//      rot9  = nod.rec[i].rot;
//      ral9  = nod.rec[i].ral;
//      dist9 = nod.rec[i].dist;
//      break;
//   }
//   if(ps9!=0){
//      for( i=0;i<=MAXPUNTINODO-1;i++){
//         if(nod.rec[i].ps!=0  ) continue;
//         nod.rec[i  ].pt   = 2447;
//         nod.rec[i  ].ps   = ps9;
//         nod.rec[i  ].av   = av9;
//         nod.rec[i  ].ind  = ind9;
//         nod.rec[i  ].rot  = rot9;
//         nod.rec[i  ].ral  = ral9;
//         nod.rec[i  ].dist = dist9;
//
//         break;
//      }
//   }


//   memcpy( N.n[nod.num], &nod, sizeof( NOD) );
//   err = N.save_record_nodi( nod.num, buffer);
//   if( err ){ FileTraccia(buffer); return;}

     for( n_rec=n_rec+1; n_rec<=MAXPUNTI; n_rec++){
        if( N.n[n_rec]->num >0 ) break;
     }
     memset(&nod, 0, sizeof(NOD));
     stato= N.cerca_record(&nod, n_rec );
     if( stato)  break;
     if( n_rec>=MAXPUNTI ) break;
  }
  if(!N.save_file_nodi( buffer )){FileTraccia("Errore salvataggio file nodi...");}
  else {FileTraccia(buffer);}
  FileTraccia("-----------------------------------------------------------------------------------------------------------------------------------------");
*/
  ShowMessage("Ok!");
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::BtnCompTuttoClick(TObject *Sender)
{
  int stato = 0, j=0;
  int i=0;
  int num0, num1, num2;
  int n_rec, err, k;
  char buffer[201]="";
  struct NOD nod;

//calcola_zona(zona[0]);
  //---------------------------------------
  // RICHIESTA CONFERMA
  //---------------------------------------
  if(Application->MessageBox("Are you sure?", "COMPILE ALL", MB_YESNO | MB_ICONQUESTION)!=IDYES) return;
  //---------------------------------------
  //  cerca il primo record
  //---------------------------------------
  for( n_rec=1; n_rec<=MAXPUNTI; n_rec++){
     if( N.n[ n_rec ]->num >0 ) break;
  }
  if( n_rec>MAXPUNTI ) return;


//TestListBox->Items->Clear();
//TestListBox->ItemIndex = 0;
  for(;;){
     stato = N.cerca_record(&nod, n_rec);
     if( stato ) {
        sprintf(buffer, "Errore: Nodo %03d non trovato in archivio!", n_rec);
        TestListBox->Items->Add(buffer);
        return;
     }
     sprintf(buffer, "Compilazione Nodo: %03d", nod.num);
   //TestListBox->Items->Strings[0]=buffer;
   //TestListBox->Items->Move(TestListBox->ItemIndex, 0);
     num0=nod.num;

     if(N.punti_notevoli[num0]){
        for(i=0; i<=MAXPUNTINODO-1; i++){
           if(nod.rec[i].zp==0 && nod.rec[i].pt==0) break;
           if(nod.rec[i].ps!=0) break;
           for(k=0;;k++){
              if( (Percorso[k][0]==0) && (Percorso[k][1]==0) ){
                 break;
              }
              if(Percorso[k][0]==num0) nod.rec[i].ps=(short int)(Percorso[k][1]);
              if(Percorso[k][1]==num0) nod.rec[i].ps=(short int)(Percorso[k][0]);
           }
        }
        memcpy( N.n[nod.num], &nod, sizeof( NOD) );
     }

     for( i=0;i<=MAXPUNTINODO-1;i++){
        pt0[i] = nod.rec[i].pt;
        zs0[i] = nod.rec[i].zs;
        zp0[i] = nod.rec[i].zp;
        ps0[i] = nod.rec[i].ps;
     }
     for( i=0;i<=MAXPUNTINODO-1;i++){
        if( ps0[i]>TOTPUNTI ){
           sprintf( buffer, "Errore: Punto sucessivo %03d non valido", ps0[i]);
           TestListBox->Items->Add(buffer);
           return;
        }
        if(ps0[i]){
           N.cerca_record(&nod, ps0[i] );
           for( j=0;j<=MAXPUNTINODO-1;j++){
              pt1[j] = nod.rec[j].pt;
              zs1[j] = nod.rec[j].zs;
              zp1[j] = nod.rec[j].zp;
              ps1[j] = nod.rec[j].ps;
           }
           if((!zs0[i])&&(zp0[i])){
              num2 = CalcolaNum2(i, num0, 11);
              if (!num2) num2 = CalcolaNum2(i, num0, 12);
           }
           if((zs0[i])&&(zp0[i])){
              num2 = CalcolaNum2(i,num0,21);
              if (!num2) num2 = CalcolaNum2(i, num0, 22);
              if (!num2) num2 = CalcolaNum2(i, num0, 23);
           }
           if(pt0[i]){
              num2 = CalcolaNum2(i,num0,31);
              if (!num2) num2 = CalcolaNum2(i, num0, 32);
              if (!num2) num2 = CalcolaNum2(i, num0, 33);
           }
           N.cerca_record(&nod, num0 );
           num1=nod.rec[i].ps;
           CalcolaRallentamento(i, num0, num2, nod.rec[i].pt, nod.rec[i].ps, &nod);
           CalcolaDirezione(i, num0, num1, &nod);
           CalcolaCaricoScarico(i, num0, &nod);
           memcpy( N.n[nod.num], &nod, sizeof( NOD) );
           err = N.save_record_nodi( nod.num, buffer);
           if( err ){
              TestListBox->Items->Add(buffer);
              return;
           }
        }
     }
   //CalcolaDatiGenerali(num0, &nod);
     memcpy( N.n[nod.num], &nod, sizeof( NOD) );
     err = N.save_record_nodi( nod.num, buffer);
     if( err ){
        TestListBox->Items->Add(buffer);
        return;
     }
     for( n_rec=n_rec+1; n_rec<=MAXPUNTI; n_rec++){
        if( N.n[n_rec]->num >0 ) break;
     }
     stato= N.cerca_record(&nod, n_rec );
     if( stato)  break;
     if( n_rec>=MAXPUNTI ) break;
  }
  if(!N.save_file_nodi( buffer )){
     TestListBox->Items->Add("Ok: Compilazione Nodi avvenuta correttamente!");
  }
  else TestListBox->Items->Add(buffer);
  TestListBox->Items->Add("-----------------------------------------------------------------------------------------------------------------------------------------");
  return;
}
//---------------------------------------------------------------------------
int TFormTest::CalcolaNum2(int i, int num0, int opt)
{
  int j, k, flag;

  switch(opt){
     case 11:
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if((zp0[i]==zp1[j])&&(num0!=ps1[j])){
              return ps1[j];
           }
        }
        break;
     case 12:
/*
        flag=0;
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if (zona[pt1[j]][0] == zp0[i]){
              for(k=1;k<=MAXPUNTINODO-1;k++){
                 if (pt0[k]==pt1[j]){
                    flag=1;
                    break;
                 }
              }
              if(flag) continue;
              else return ps1[j];
           }
        }
*/
        break;
     case 21:
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if((zp0[i]==zp1[j])&&(zs0[i]==zs1[j])&&(num0!=ps1[j])){
              return ps1[j];
           }
        }
        return 0;
     case 22:
/*
        flag=0;
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if(pt1[j]){
              if((zona[pt1[j]][0] == zp0[i])&&(zona[pt1[j]][1] == zs0[i])){
                 for(k=1;k<=MAXPUNTINODO-1;k++){
                    if(pt0[k]==pt1[j]){
                       flag=1;
                       break;
                    }
                 }
                 if(flag) continue;
                 else return ps1[j];
              }
           }
        }
*/
        return 0;
     case 23:
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if((zp0[i]==zp1[j])&&(!zs1[j])){
              return ps1[j];
           }
        }
        return 0;
     case 31:
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if(pt0[i]==pt1[j]){
              return ps1[j];
           }
        }
        return 0;
     case 32:
/*
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if((zona[pt0[i]][0] == zp1[j])&&(zona[pt0[i]][1] == zs1[j])){
              return ps1[j];
           }

        }
*/
        return 0;
     case 33:
/*
        for(j=0;j<=MAXPUNTINODO-1;j++){
           if((zona[pt0[i]][0] == zp1[j])&&(!zs1[j])){
              return ps1[j];
           }
        }
*/
        return 0;
  }
  return 0;
}
//---------------------------------------------------------------------------
void TFormTest::CalcolaRallentamento(int i, int n0, int n2, int pt0, int ps0, struct NOD *nod)
{
  int x_n0, y_n0;
  int x_n1, y_n1;
  int distX, distY;
  int dist, id_curva, raggio, circonferenza;

  dist=0;
  //------------------------------------------------------
  // Viene messo di default il rallentamento a "9" perchè
  // il rallentamento corretto viene calcolato in fase di
  // esecuzione della missione.
  //------------------------------------------------------
  x_n0 = Punti[n0][0];
  y_n0 = Punti[n0][1];
  x_n1 = Punti[ps0][0];
  y_n1 = Punti[ps0][1];
  //------------------------------------------------------
  // Se i nodi si trovano in retta la distanza viene
  // calcolata con il teorema di pitagora.
  //------------------------------------------------------
  distX=abs(x_n0-x_n1);
  distY=abs(y_n0-y_n1);
  if(distX<30000 && distY<30000) dist = (int)(sqrt((distX*distX)+(distY*distY)));
  //------------------------------------------------------
  // Calcolo della distanza percorsa su un arco di
  // circonferneza.
  //------------------------------------------------------
  id_curva=raggio=circonferenza=0;
  if(!N.CercaCurva(TRUE, n0, ps0, &raggio, &circonferenza, &id_curva)){
     dist   = (int)(circonferenza);
  }
  nod->rec[i].dist= (short int)(dist);
  //------------------------------------------------------
  // Verifica che il valore della distanza stia nel range
  //------------------------------------------------------
  if((nod->rec[i].dist<= 0 ) || (nod->rec[i].dist>16000*RAP_ENCODER)){
     nod->rec[i].dist = 25;
  }
  //------------------------------------------------------
  // Rallentamento a "9" per il calcolo automatico
  //------------------------------------------------------
  nod->rec[i].ral=9;
  return;
}
//---------------------------------------------------------------------------
void TFormTest::CalcolaDirezione(int i, int n0, int n1, struct NOD *nod)
{
  char buffer[101]="";
  int  offx, offy;
  short int num_mac, num_mag, num_pos, num_pin;
  bool SucPT=false;
  bool AttPT=false;
  bool Fifo=false;
  bool trovato, prec_prec_a_pt;
  bool x0_uguale_xv, x1_uguale_xv, y0_uguale_yv, y1_uguale_yv;
  int nod_out=0;
  int j, x_n0, x_n1, y_n0, y_n1;
  int diff_x, diff_y;
  int quadrante;

  const int TRUEA  = 1;
  const int FALSEA = 0;

  const int ROT0   = 0;
  const int ROT90  = 1;
  const int ROT180 = 2;
  const int ROT270 = 3;

  offx=offy=0;
  SucPT=AttPT=Fifo=false;
  //--------------------------------------
  // PUNTI TERMINALI
  // Direzione  AVANTI in ingresso
  // Direzione  INDIETTRO in uscita
  //--------------------------------------
  if(N.punti_notevoli[n1]) SucPT=true;
  if(N.punti_notevoli[n0]){
     AttPT=true;
     //-----------------------------------------
     // memorizzo il nodo di uscita prendendolo
     // dalla prima riga
     //-----------------------------------------
     nod_out = nod->rec[0].ps;
     //-----------------------------------------
     // Verifico se il nodo appartiene a una
     // struttura di tipo FIFO
     //-----------------------------------------
     P.test_punto_presente_mac(buffer,  (short int)(n0), &num_mac, &num_pos);
     M.test_punto_presente_mag(buffer,  (short int)(n0), &num_mag, &num_pos);
     // MACCHINE PASSANTI
     if(num_mac>0 && num_mac<=TOTCHIAMATE){
        if(P.ch[num_mac]->Staz[num_pos].PuntoIng!=0 && P.ch[num_mac]->Staz[num_pos].PuntoOut!=0){
           if(P.ch[num_mac]->Staz[num_pos].PuntoIng!=P.ch[num_mac]->Staz[num_pos].PuntoOut) Fifo=true;
        }
     }
     // MAGAZZINI PASSANTI
     // trattandosi di impianti con navette pallet gli ingressi vengono gestiti sempre
     // in modalità LIFO
   //if(num_mag>0 && num_mag<=TOTMAG){
   //   if(M.m[num_mag]->PuntoIng!=M.m[num_mag]->PuntoOut) Fifo=true;
   //}
  }

  //---------------------------------------------------
  // 30/04/97 Coordinate grafiche assolute
  //---------------------------------------------------
  x_n0 = Punti[n0][0];
  y_n0 = Punti[n0][1];
  x_n1 = Punti[n1][0];
  y_n1 = Punti[n1][1];
  offx = abs(x_n0-x_n1);
  offy = abs(y_n0-y_n1);
  //--------------------------------------------------
  // Se mi trovo su un PT e il succ è anch'esso un PT
  // LIFO - proseguo avanti solo se il PT SUCC è
  //        nella direzione opposta all'uscita.
  // FIFO - proseguo avanti.
  //--------------------------------------------------
  if(SucPT && AttPT && nod_out!=0){
     if(!Fifo){
        if(n1!=nod_out){
           if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT0;
           if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT270;
           if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT180;
           if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT90;
           nod->rec[i].av =TRUEA;
           nod->rec[i].ind=FALSEA;
        }
        else{
           if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT180;
           if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT90;
           if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT0;
           if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT270;
           nod->rec[i].av =FALSEA;
           nod->rec[i].ind=TRUEA;
        }
     }
     if(Fifo){
        if(n1==nod_out){
           if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT0;
           if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT270;
           if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT180;
           if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT90;
           nod->rec[i].av =TRUEA;
           nod->rec[i].ind=FALSEA;
        }
        else{
           if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT180;
           if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT90;
           if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT0;
           if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT270;
           nod->rec[i].av =FALSEA;
           nod->rec[i].ind=TRUEA;
        }
     }
     return;
  }


  //--------------------------------------------------
  // Se non mi trovo su un PT ma il succ lo è
  // LIFO - entro avanti
  // FIFO - entro avanti
  //--------------------------------------------------
  if(SucPT && !AttPT){
     if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT0;
     if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT270;
     if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT180;
     if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT90;
     nod->rec[i].av =TRUEA;
     nod->rec[i].ind=FALSEA;
     return;
  }

  //--------------------------------------------------
  // Se non mi trovo su un PT e il succ non lo è:
  // LIFO - per uscire torno indierto.
  // FIFO - per uscire proseguo avanti
  //--------------------------------------------------
  if(AttPT && !SucPT && (nod_out!=0 && n1==nod_out)){
     if(Fifo==false){
        if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT180;
        if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT90;
        if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT0;
        if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT270;
        nod->rec[i].av =FALSEA;
        nod->rec[i].ind=TRUEA;
     }
     else{
        if(offx<=5 && y_n1<y_n0) nod->rec[i].rot=ROT0;
        if(offy<=5 && x_n1>x_n0) nod->rec[i].rot=ROT270;
        if(offx<=5 && y_n1>y_n0) nod->rec[i].rot=ROT180;
        if(offy<=5 && x_n1<x_n0) nod->rec[i].rot=ROT90;
        nod->rec[i].av =TRUEA;
        nod->rec[i].ind=FALSEA;
     }
     return;
  }

  //---------------------------------------------------
  // Altrimenti sempre  -> Direzione INDIETRO
  //---------------------------------------------------
  if(!AttPT && !SucPT){
     //-------------------------------------
     // Direzione INDIETRO
     //-------------------------------------
     nod->rec[i].av =FALSEA;
     nod->rec[i].ind=TRUEA;
     //-------------------------------------
     // Inversione di rotazione per ing. PT
     //-------------------------------------
     prec_prec_a_pt=false;
     if(nod->rec[i].pt!=0){
        for(j=0; j<MAXPUNTINODO; j++){
           if(N.n[n1]->rec[j].ps==0                 ) break;
           if(N.n[n1]->rec[j].pt==0                 ) continue;
           if(N.n[n1]->rec[j].pt!=N.n[n1]->rec[j].ps) continue;
           //----------------------------------------
           // Nodo considerato prec_prec a PT
           //----------------------------------------
           if(N.n[n1]->rec[j].pt==nod->rec[i].pt){
              prec_prec_a_pt=true;
              break;
           }
        }
     }
     //-------------------------------------
     // Recupero rotazione su tratti retti
     //-------------------------------------
     if(offx<=5 && y_n0>y_n1) nod->rec[i].rot=ROT180;
     if(offy<=5 && x_n0<x_n1) nod->rec[i].rot=ROT90;
     if(offx<=5 && y_n0<y_n1) nod->rec[i].rot=ROT0;
     if(offy<=5 && x_n0>x_n1) nod->rec[i].rot=ROT270;
     //-------------------------------------
     // TRATTI INCLINATI
     // Assegnazione "approsimativa" di
     // gradi in deviazione
     //-------------------------------------
     diff_x = abs(x_n0-x_n1);
     diff_y = abs(y_n0-y_n1);
     if(diff_y!=0 && diff_x!=0){
        quadrante=0;
        trovato=false;
        //-------------------------------------
        // CRUVE DI BEZIER
        //-------------------------------------
        for(j=0;;j++){
           if(trovato==true                                                                                ) break;
           if(PercorsoCurvaBezier[j][0]==0  && PercorsoCurvaBezier[j][1]==0 && PercorsoCurvaBezier[j][2]==0 ) break;
           if(PercorsoCurvaBezier[j][1]!=n0 && PercorsoCurvaBezier[j][2]!=n0) continue;
           if(PercorsoCurvaBezier[j][1]!=n1 && PercorsoCurvaBezier[j][2]!=n1) continue;
           trovato=true;
           //----------------------------------------------------------
           // Se il nodo considerato è il prec_prec a un PT devo
           // eseguire un'inversione di rotazione
           //----------------------------------------------------------
           // 08/10/2014 - IN ATTESA DI VALUTAZIONI
           /*
           if(prec_prec_a_pt){
              //-------------------------------------
              // Direzione ABANTI
              //-------------------------------------
              nod->rec[i].av =TRUEA;
              nod->rec[i].ind=FALSEA;
           }
           */
           //----------------------------------------------------------
           // RECUPERO IL QUADRANTE       _         _
           // Quadrante (dal vartice) 1 =  |   2 = |    3 = |_   4 = _|
           //----------------------------------------------------------
           x0_uguale_xv=x1_uguale_xv=y0_uguale_yv=y1_uguale_yv=false;
           if(abs(x_n0-PercorsoCurvaBezier[j][3])<5) x0_uguale_xv=true;
           if(abs(x_n1-PercorsoCurvaBezier[j][3])<5) x1_uguale_xv=true;
           if(abs(y_n0-PercorsoCurvaBezier[j][4])<5) y0_uguale_yv=true;
           if(abs(y_n1-PercorsoCurvaBezier[j][4])<5) y1_uguale_yv=true;

           if(x0_uguale_xv==true  && x1_uguale_xv==false && y0_uguale_yv==false && y1_uguale_yv==true && x_n1<PercorsoCurvaBezier[j][3] && y_n0>PercorsoCurvaBezier[j][4]) quadrante=1;
           if(x0_uguale_xv==false && x1_uguale_xv==true  && y0_uguale_yv==true  && y1_uguale_yv==false && x_n0<PercorsoCurvaBezier[j][3] && y_n1>PercorsoCurvaBezier[j][4]) quadrante=1;

           if(x0_uguale_xv==true  && x1_uguale_xv==false && y0_uguale_yv==false && y1_uguale_yv==true && x_n1>PercorsoCurvaBezier[j][3] && y_n0>PercorsoCurvaBezier[j][4]) quadrante=2;
           if(x0_uguale_xv==false && x1_uguale_xv==true  && y0_uguale_yv==true  && y1_uguale_yv==false && x_n0>PercorsoCurvaBezier[j][3] && y_n1>PercorsoCurvaBezier[j][4]) quadrante=2;

           if(x0_uguale_xv==true  && x1_uguale_xv==false && y0_uguale_yv==false && y1_uguale_yv==true && x_n1>PercorsoCurvaBezier[j][3] && y_n0<PercorsoCurvaBezier[j][4]) quadrante=3;
           if(x0_uguale_xv==false && x1_uguale_xv==true  && y0_uguale_yv==true  && y1_uguale_yv==false && x_n0>PercorsoCurvaBezier[j][3] && y_n1<PercorsoCurvaBezier[j][4]) quadrante=3;

           if(x0_uguale_xv==true  && x1_uguale_xv==false && y0_uguale_yv==false && y1_uguale_yv==true && x_n1<PercorsoCurvaBezier[j][3] && y_n0<PercorsoCurvaBezier[j][4]) quadrante=4;
           if(x0_uguale_xv==false && x1_uguale_xv==true  && y0_uguale_yv==true  && y1_uguale_yv==false && x_n0<PercorsoCurvaBezier[j][3] && y_n1<PercorsoCurvaBezier[j][4]) quadrante=4;

           switch(quadrante){
              case  1: if(x_n0<x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0>x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT180;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT180;
                       break;
              case  2: if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT180;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT270;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT180;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT270;
                       break;
              case  3: if(x_n0>x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT270;
                       if(x_n0<x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT0;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT0;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT270;
                       break;
              case  4: if(x_n0>x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT0;
                       if(x_n0<x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT0;
                       break;
              default: break;
           }

           break;
        }
        //-------------------------------------
        // CRUVE ARCO DI CRF
        //-------------------------------------
        for(j=0;;j++){
           if(trovato==true                                     ) break;
           if(PercorsoCurva[j][0]==0  && PercorsoCurva[j][1]==0 ) break;
           if(PercorsoCurva[j][0]!=n0 && PercorsoCurva[j][1]!=n0) continue;
           if(PercorsoCurva[j][0]!=n1 && PercorsoCurva[j][1]!=n1) continue;
           trovato=true;
           //----------------------------------------------------------
           // Se il nodo considerato è il prec_prec a un PT devo
           // eseguire un'inversione di rotazione
           //----------------------------------------------------------
           // 08/10/2014 - IN ATTESA DI VALUTAZIONI
           /*
           if(prec_prec_a_pt){
              //-------------------------------------
              // Direzione ABANTI
              //-------------------------------------
              nod->rec[i].av =TRUEA;
              nod->rec[i].ind=FALSEA;
           }
           */
           //----------------------------------------------------------
           // RECUPERO IL QUADRANTE      _         _
           // Quadrante:             1 =  |   2 = |    3 = |_   4 = _|
           //----------------------------------------------------------
           quadrante = PercorsoCurva[j][2];
           switch(quadrante){
              case  1: if(x_n0<x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0>x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT180;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT180;
                       break;
              case  2: if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT180;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT270;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT180;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT270;
                       break;
              case  3: if(x_n0>x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT270;
                       if(x_n0<x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT0;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT0;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT270;
                       break;
              case  4: if(x_n0>x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT0;
                       if(x_n0<x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0<x_n1 && y_n0>y_n1) nod->rec[i].rot=ROT90;
                       if(x_n0>x_n1 && y_n0<y_n1) nod->rec[i].rot=ROT0;
                       break;
              default: break;
           }
           break;
        }
        //-------------------------------------
        // TRATTI INCLINATI
        // (solo se non appartiene a curve)
        //-------------------------------------
        if(trovato==false){
           if(diff_x>diff_y){
              if(x_n0<x_n1) nod->rec[i].rot=ROT90;
              if(x_n0>x_n1) nod->rec[i].rot=ROT270;
           }
           if(diff_y>diff_x){
              if(y_n0<y_n1) nod->rec[i].rot=ROT0;
              if(y_n0>y_n1) nod->rec[i].rot=ROT180;
           }
        }
     }
  }
}
//---------------------------------------------------------------------------
void TFormTest::CalcolaCaricoScarico(int i, int n0, struct NOD *nod)
{
  const int TRUEA      = 1;
  const int FALSEA     = 0;
  short int num_mag, num_pos;
  bool calcolo_dir_car_scar;
  char buffer[101];

  calcolo_dir_car_scar=false;
  //------------------------------------------
  // La direzione di carico/scarico è solo
  // per i magazzini a profondità
  //------------------------------------------
  if(N.punti_notevoli[n0]==true){
     num_mag=num_pos=0;
     M.test_punto_presente_mag(buffer, (short int)(n0), &num_mag, &num_pos);
     if(num_mag!=0 && num_pos==0) calcolo_dir_car_scar=true;
  }
  //------------------------------------------
  // Solo per struttue abilitate
  // (magazzini a profondità)
  //------------------------------------------
  if(calcolo_dir_car_scar==true){
     if(nod->rec[i].av==TRUEA && nod->rec[i].ind==FALSEA){
        nod->direz=2;
     }
     if(nod->rec[i].av==FALSEA && nod->rec[i].ind==TRUEA){
        nod->direz=1;
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::BtnStampaClick(TObject *Sender)
{
  bool OkStampa;

  OkStampa = PrintDialog->Execute();
  if(OkStampa==false) return;
  FormTest->Print();
}
//---------------------------------------------------------------------------

