//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GrpDest.h"
#include "TecnoFer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGrpDest *FormGrpDest;
//---------------------------------------------------------------------------
__fastcall TFormGrpDest::TFormGrpDest(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGrpDest::FormActivate(TObject *Sender)
{
  char buffer[101]="";
  short int i, magazzino;
  bool solo_svuotamento;

  //---------------------------------------------------------
  // Caption
  //---------------------------------------------------------
  sprintf(buffer, "Destination for pallets in store n.%d", FormGrpDest->NumMagazzino);
  FormGrpDest->Caption = buffer;
  
  //---------------------------------------------------------
  // Abilitazione solo svuotamento
  //---------------------------------------------------------
  solo_svuotamento=false;
  magazzino=FormGrpDest->NumMagazzino;
  if(magazzino>0 && magazzino<=TOTMAG){
     if(M.m[magazzino]->Tipo==MG_TEMPORANEO) solo_svuotamento=true;
  }
  //---------------------------------------------------------
  // Andrea 31/0/2018 - Abilita/Disabilita Gruppo mag.
  // Se AbilitaSvuotamentoManualeMag==4 allora inserisce
  // Svuotamento Mag.   =0
  // Disabil.Gruppo Mag.=1
  // Abilita.Gruppo Mag.=2
  //---------------------------------------------------------
  Group=NN;
  QtyPrel=0;
  CBGruppoDest->Clear();
  CBGruppoDest->Items->Add(MESS[12]);    // SVUOTAMENTO
  if(solo_svuotamento==false){
     CBGruppoDest->Items->Add(MESS[14]);    // DISABILITA GRUPPO MAG
     CBGruppoDest->Items->Add(MESS[15]);    // ABILITA GRUPPO MAG
     if( AbilitaSvuotamentoManualeMag>1 ){
        //-----------------------------------
        // Compliazione array GRUPPI AMMESSI
        //-----------------------------------
        for(i=1; i<MAXGRUPPIIMP; i++){
           if(strcmp(P.GruppiBaie[i].Nome, "")==0) continue;
           CBGruppoDest->Items->Add(P.GruppiBaie[i].Nome);
        }
     }
  }
  CBGruppoDest->ItemIndex=Group;
  EdQty->Visible = false;
  TextQty->Visible = false;
  FormGrpDest->Height=152;
}
//---------------------------------------------------------------------------
void __fastcall TFormGrpDest::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  FormGrpDest->NumMagazzino=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormGrpDest::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Key==VK_ESCAPE ) Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormGrpDest::BtnOkClick(TObject *Sender)
{
  int i, j;
  AnsiString Buff;

  Group = CBGruppoDest->ItemIndex;
  //----------------------------------------
  // Verifica Quantità da spostare
  //----------------------------------------
  QtyPrel =(short int)(atoi(EdQty->Text.c_str()));
  //----------------------------------------
  // Reset elenco posizioni da movimentare
  //----------------------------------------
  for(i=0; i<(MAXTERRAMAG*MAXPIANIMAG); i++) PosToMove[i]="";
  //----------------------------------------
  // Elenco Posizioni da movimentare
  //----------------------------------------
  i=0;
  for(j=0; j<ChListUdc->Items->Count; j++){
     if(ChListUdc->Checked[j]==false) continue;
     PosToMove[i] = ChListUdc->Items->Strings[j].SubString(2, 5);
     i++;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormGrpDest::BtnCloseClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormGrpDest::CBGruppoDestChange(TObject *Sender)
{
  bool fifo;
  char buffer[1001];
  char buffer1[101];
  short int n_cestoni;
  short int magazzino;
  short int Pos, Pin;
  short int i, j, k;

  //---------------------------------------------------------
  // Abilitazione solo svuotamento
  //---------------------------------------------------------
  fifo=false;
  n_cestoni=0;
  magazzino=FormGrpDest->NumMagazzino;
  if(magazzino>0 && magazzino<=TOTMAG){
     if(M.m[magazzino]->Tipo!=MG_TEMPORANEO && M.m[magazzino]->Tipo!=MG_VUOTI){
        n_cestoni=M.m[magazzino]->NumCestoni;
        fifo     =M.m[magazzino]->Fifo;
     }
  }
  //---------------------------------------------------------
  // Gruppo
  //---------------------------------------------------------
  Group = CBGruppoDest->ItemIndex;
  if( Group<2 ){
     EdQty->Visible = false;
     TextQty->Visible = false;
  }
  else{
     EdQty->Visible = true;
     TextQty->Visible = true;
  }
  //---------------------------------------------------------
  // GESTIONE UDC
  //---------------------------------------------------------
  if(GestioneUdc!=0 && n_cestoni!=0 && Group>=2){
     FormGrpDest->Height=457;
     ChListUdc->Clear();
     memset(&buffer[0], 0, sizeof(buffer));
     // LIFO
     if(fifo==false){
        for(Pos=1; Pos<=MAXTERRAMAG; Pos++){
           for(Pin=MAXPIANIMAG-1; Pin>=0; Pin--){
              if(M.m[magazzino]->Pos[Pos].Presenza[Pin]==false) continue;
              for(i=0; i<POSINPRESA; i++){
                 for(j=0; j<PIANIINPRESA; j++){
                    if(M.m[magazzino]->Pos[Pos].Box[Pin].Presa[i][j]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    k=i+(j*POSINPRESA);
                    if(k==0){
                       sprintf(buffer, "(%02d,%02d)  -  %s", Pos, Pin, M.m[magazzino]->Pos[Pos].Box[Pin].Prd[k].Udc);
                    }
                    else{
                       sprintf(buffer1, ", %s", M.m[magazzino]->Pos[Pos].Box[Pin].Prd[k].Udc);
                       strcat(buffer, buffer1);
                    }
                 }
              }
              ChListUdc->Items->Add(buffer);
           }
        }
     }
     // FIFO
     else{
        for(Pos=MAXTERRAMAG; Pos>0; Pos--){
           for(Pin=MAXPIANIMAG-1; Pin>=0; Pin--){
              if(M.m[magazzino]->Pos[Pos].Presenza[Pin]==false) continue;
              for(i=0; i<POSINPRESA; i++){
                 for(j=0; j<PIANIINPRESA; j++){
                    if(M.m[magazzino]->Pos[Pos].Box[Pin].Presa[i][j]==false) continue;
                    //-------------------------------------------
                    // Verifico se UDC duplicato
                    //-------------------------------------------
                    k=i+(j*POSINPRESA);
                    if(k==0){
                       sprintf(buffer, "(%02d,%02d)  -  %s", Pos, Pin, M.m[magazzino]->Pos[Pos].Box[Pin].Prd[k].Udc);
                    }
                    else{
                       sprintf(buffer1, ", %s", M.m[magazzino]->Pos[Pos].Box[Pin].Prd[k].Udc);
                       strcat(buffer, buffer1);
                    }
                 }
              }
              ChListUdc->Items->Add(buffer);
           }
        }
     }
  }
  else{
     FormGrpDest->Height=152;
     ChListUdc->Clear();
  }
}
//---------------------------------------------------------------------------


