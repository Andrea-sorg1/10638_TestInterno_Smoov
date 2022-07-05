//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ElCar2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormElBox *FormElBox;
//---------------------------------------------------------------------------
__fastcall TFormElBox::TFormElBox(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormElBox::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------

void __fastcall TFormElBox::FormActivate(TObject *Sender)
{
  BtnEsci->Caption = MESS[171];
  //-----------------------------
  // Refresh della form
  //-----------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormElBox::BtnOkClick(TObject *Sender)
{
  //-------------------------------
  // Aggiornamento della struttura
  //-------------------------------
  GetCaselle();
}
//---------------------------------------------------------------------------
// ---------------------------
//    SetCaselle()
// ---------------------------
// Aggiornamento oggetti form
//
void __fastcall TFormElBox::SetCaselle()
{
  int i;
  
  //------------------------------------------------
  // elenco carrelli
  //------------------------------------------------
  for(i=0; i<MAXLISTABOX; i++) GridElenco->Cells[0][i] = ElencoBox[i];
}

//---------------------------------------------------------------------------
// ---------------------------
//    GetCaselle()
// ---------------------------
// Aggiornamento oggetti form
//
void __fastcall TFormElBox::GetCaselle()
{
  int i;
  bool lista_ok;

  //------------------------------------------------
  // elenco carrelli
  //------------------------------------------------
  for(i=0; i<MAXLISTABOX; i++) ElencoBox[i] = (short int)(atoi(GridElenco->Cells[0][i].c_str()));
  //------------------------------------------------
  // Eliminazione eventuali buchi nella lista
  //------------------------------------------------
  lista_ok=true;
  do{
     for(i=0; i<MAXLISTABOX-1; i++){
        if(ElencoBox[i]>0 && ElencoBox[i]<=MAXBOXIMPIANTO) continue;
        if(ElencoBox[i+1]==0                             ) continue;
        if(ElencoBox[i+1]>MAXBOXIMPIANTO                 ) continue;
        ElencoBox[i]   = ElencoBox[i+1];
        ElencoBox[i+1] = 0;
        lista_ok=false;
        break;
     }
  }while(lista_ok==false && i<MAXLISTABOX-1);
}
//---------------------------------------------------------------------------

void __fastcall TFormElBox::BtnRESETClick(TObject *Sender)
{
  int i;

  //----------------------------------
  // Azzeramento lista
  //----------------------------------
  for(i=0; i<MAXLISTABOX; i++) ElencoBox[i]=0;
  //----------------------------------
  // Refresh
  //----------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------

