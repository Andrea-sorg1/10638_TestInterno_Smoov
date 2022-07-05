//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TecnoFer.h"
#include "Asservi.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAsservi *FormAsservi;
//---------------------------------------------------------------------------
__fastcall TFormAsservi::TFormAsservi(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::FormActivate(TObject *Sender)
{
  int  i, j;
  int  TotMac  = TOTCHIAMATE;
  int  TotBaie = TOTBAIE;
  char all_mess[101]="";
  //---------------------------------
  // Controllo validità numero TGV
  //---------------------------------
  if(NumTgv<1       ) Close();
  if(NumTgv>MAXAGV  ) Close();
  //---------------------------------
  // Array di oggetti
  //---------------------------------
  Gruppi();
  //---------------------------------
  // Gestione messaggeria
  //---------------------------------
  Messaggi();
  //---------------------------------
  // Compilazione dei combo box
  //---------------------------------
  if(TotMac<1){
     GBMac->Enabled=false;
     for(i=1; i<21; i++){
        CBMac[i]->Color = clLtGray;
     }
  }
  else{
     for(i=1; i<21; i++){
        CBMac[i]->Items->Clear();
        CBMac[i]->Items->Add(MESS[521]);
        for(j=1; j<=TOTCHIAMATE; j++){
           // Macchine inserite
           if(P.ch[j]->Num>0 && P.ch[j]->Num<=TOTCHIAMATE){
              sprintf(all_mess, "%02d - %s", P.ch[j]->Num, P.ch[j]->Sigla);
              CBMac[i]->Items->Add(all_mess);
           }
           // Macchine future
           else{
              sprintf(all_mess, "%02d - unknow", j);
              CBMac[i]->Items->Add(all_mess);
           }
        }
     }
  }
  //---------------------------------
  // Compilazione dei combo box
  //---------------------------------
  if(TotBaie<1){
     GBBaie->Enabled=false;
     for(i=1; i<41; i++){
        CBBai[i]->Color = clLtGray;
     }
  }
  else{
     for(i=1; i<41; i++){
        CBBai[i]->Items->Clear();
        for(j=0; j<MAXGRUPPIIMP; j++){
           CBBai[i]->Items->Add(P.GruppiBaie[j].Nome);
        }   
        CBBai[i]->ItemIndex=NN;
     }
  }
  //---------------------------------
  // Refresh della form
  //---------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  if( Key == VK_ESCAPE                    ) Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::Gruppi()
{
  CBMac[ 1] = EditComboMac1;
  CBMac[ 2] = EditComboMac2;
  CBMac[ 3] = EditComboMac3;
  CBMac[ 4] = EditComboMac4;
  CBMac[ 5] = EditComboMac5;
  CBMac[ 6] = EditComboMac6;
  CBMac[ 7] = EditComboMac7;
  CBMac[ 8] = EditComboMac8;
  CBMac[ 9] = EditComboMac9;
  CBMac[10] = EditComboMac10;
  CBMac[11] = EditComboMac11;
  CBMac[12] = EditComboMac12;
  CBMac[13] = EditComboMac13;
  CBMac[14] = EditComboMac14;
  CBMac[15] = EditComboMac15;
  CBMac[16] = EditComboMac16;
  CBMac[17] = EditComboMac17;
  CBMac[18] = EditComboMac18;
  CBMac[19] = EditComboMac19;
  CBMac[20] = EditComboMac20;

  CBing[ 1] = CBing1;
  CBing[ 2] = CBing2;
  CBing[ 3] = CBing3;
  CBing[ 4] = CBing4;
  CBing[ 5] = CBing5;
  CBing[ 6] = CBing6;
  CBing[ 7] = CBing7;
  CBing[ 8] = CBing8;
  CBing[ 9] = CBing9;
  CBing[10] = CBing10;
  CBing[11] = CBing11;
  CBing[12] = CBing12;
  CBing[13] = CBing13;
  CBing[14] = CBing14;
  CBing[15] = CBing15;
  CBing[16] = CBing16;
  CBing[17] = CBing17;
  CBing[18] = CBing18;
  CBing[19] = CBing19;
  CBing[20] = CBing20;

  CBout[ 1] = CBout1;
  CBout[ 2] = CBout2;
  CBout[ 3] = CBout3;
  CBout[ 4] = CBout4;
  CBout[ 5] = CBout5;
  CBout[ 6] = CBout6;
  CBout[ 7] = CBout7;
  CBout[ 8] = CBout8;
  CBout[ 9] = CBout9;
  CBout[10] = CBout10;
  CBout[11] = CBout11;
  CBout[12] = CBout12;
  CBout[13] = CBout13;
  CBout[14] = CBout14;
  CBout[15] = CBout15;
  CBout[16] = CBout16;
  CBout[17] = CBout17;
  CBout[18] = CBout18;
  CBout[19] = CBout19;
  CBout[20] = CBout20;

  CBBai[ 1] = EditComboBaie1;
  CBBai[ 2] = EditComboBaie2;
  CBBai[ 3] = EditComboBaie3;
  CBBai[ 4] = EditComboBaie4;
  CBBai[ 5] = EditComboBaie5;
  CBBai[ 6] = EditComboBaie6;
  CBBai[ 7] = EditComboBaie7;
  CBBai[ 8] = EditComboBaie8;
  CBBai[ 9] = EditComboBaie9;
  CBBai[10] = EditComboBaie10;
  CBBai[11] = EditComboBaie11;
  CBBai[12] = EditComboBaie12;
  CBBai[13] = EditComboBaie13;
  CBBai[14] = EditComboBaie14;
  CBBai[15] = EditComboBaie15;
  CBBai[16] = EditComboBaie16;
  CBBai[17] = EditComboBaie17;
  CBBai[18] = EditComboBaie18;
  CBBai[19] = EditComboBaie19;
  CBBai[20] = EditComboBaie20;
  CBBai[21] = EditComboBaie21;
  CBBai[22] = EditComboBaie22;
  CBBai[23] = EditComboBaie23;
  CBBai[24] = EditComboBaie24;
  CBBai[25] = EditComboBaie25;
  CBBai[26] = EditComboBaie26;
  CBBai[27] = EditComboBaie27;
  CBBai[28] = EditComboBaie28;
  CBBai[29] = EditComboBaie29;
  CBBai[30] = EditComboBaie30;
  CBBai[31] = EditComboBaie31;
  CBBai[32] = EditComboBaie32;
  CBBai[33] = EditComboBaie33;
  CBBai[34] = EditComboBaie34;
  CBBai[35] = EditComboBaie35;
  CBBai[36] = EditComboBaie36;
  CBBai[37] = EditComboBaie37;
  CBBai[38] = EditComboBaie38;
  CBBai[39] = EditComboBaie39;
  CBBai[40] = EditComboBaie40;

}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::Messaggi()
{
  int i;

  FormAsservi->Caption = MESS[535];
  GBMac->Caption       = MESS[537];
  GBBaie->Caption      = MESS[715];

  //-----------------------------------
  // Aggiornamento della messaggeria
  //-----------------------------------
  for(i=1; i<=20; i++){
     CBing[i]->Caption = MESS[538];
     CBout[i]->Caption = MESS[539];
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::SetCaselle()
{
  int i;

  //----------------------------------
  // MACCHINE
  //----------------------------------
  for(i=1; i<=TOTCHIAMATE; i++){
     CBMac[i]->ItemIndex = AGV[NumTgv]->mission.AsservMac[i];
     //-----------------------------------------------
     // Gestione tipo asservimento
     //-----------------------------------------------
     if(AGV[NumTgv]->mission.TipoAssMac[i]==ASS_MAC_TUTTE   ){ CBing[i]->Checked=false; CBout[i]->Checked=false; }
     if(AGV[NumTgv]->mission.TipoAssMac[i]==ASS_MAC_SOLO_ING){ CBing[i]->Checked=true;  CBout[i]->Checked=false; }
     if(AGV[NumTgv]->mission.TipoAssMac[i]==ASS_MAC_SOLO_OUT){ CBing[i]->Checked=false; CBout[i]->Checked=true;  }
  }
  //----------------------------------
  // BAIE
  //----------------------------------
  for(i=1; i<=MAXGRUPPIIMP; i++){
     CBBai[i]->ItemIndex = AGV[NumTgv]->mission.AsservBaie[i-1];
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::GetCaselle()
{
  int i;

  //----------------------------------
  // MACCHINE
  //----------------------------------
  for(i=1; i<=TOTCHIAMATE; i++){
     AGV[NumTgv]->mission.AsservMac[i] = (short int)(CBMac[i]->ItemIndex);
     //-----------------------------------------------
     // Gestione tipo asservimento
     //-----------------------------------------------
     if(CBing[i]->Checked==false && CBout[i]->Checked==false) AGV[NumTgv]->mission.TipoAssMac[i]=ASS_MAC_TUTTE;
     if(CBing[i]->Checked==true  && CBout[i]->Checked==false) AGV[NumTgv]->mission.TipoAssMac[i]=ASS_MAC_SOLO_ING;
     if(CBing[i]->Checked==false && CBout[i]->Checked==true ) AGV[NumTgv]->mission.TipoAssMac[i]=ASS_MAC_SOLO_OUT;
     if(CBing[i]->Checked==true  && CBout[i]->Checked==true ) AGV[NumTgv]->mission.TipoAssMac[i]=ASS_MAC_TUTTE;      // impossibile
     if(AGV[NumTgv]->mission.AsservMac[i]==NN               ) AGV[NumTgv]->mission.TipoAssMac[i]=ASS_MAC_TUTTE;
  }
  //----------------------------------
  // BAIE
  //----------------------------------
  for(i=1; i<=MAXGRUPPIIMP; i++){
     AGV[NumTgv]->mission.AsservBaie[i-1] = (short int)(CBBai[i]->ItemIndex);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormAsservi::BtnOKClick(TObject *Sender)
{
  char buffer[101]="";

  //----------------------------------------------
  // Aggiornamento struttura
  //----------------------------------------------
  GetCaselle();

  //----------------------------------------------
  // Traccia su log operatore
  //----------------------------------------------
  sprintf(buffer, "(EDITTGV) - Modifica asservimenti TGV%d", NumTgv);
  FileLogOperatore(buffer);
}
//---------------------------------------------------------------------------


