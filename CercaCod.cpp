//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CercaCod.h"
#include "SQLDB.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormCercaCod *FormCercaCod;
//---------------------------------------------------------------------------
__fastcall TFormCercaCod::TFormCercaCod(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormCercaCod::FormActivate(TObject *Sender)
{
  int  err, i, j;
  int  NumRec;
  char all_mess[101]="";
  struct DATI_PRD PRD[MAXCODICI+1];

  err=0;
  NumRec=0;
  EditCod=true;
  EditDes=false;
  memset(&PRD, 0, sizeof(PRD));
  //-----------------------------------------------
  // dimensionamento grigli in base a num record
  //-----------------------------------------------
  CercaGrid->ColWidths[0] = 20;
  CercaGrid->ColWidths[1] = 60;
  CercaGrid->ColWidths[2] = 160;
  //-----------------------------------------------
  // Compilazione prima riga lista prodotti
  //-----------------------------------------------
  j=0;
  CercaGrid->Cells[0][j] = "0";
  CercaGrid->Cells[1][j] = "";
  CercaGrid->Cells[2][j] = "";
  //-----------------------------------------------
  // se la comunicazione con DBTECNO è disattivata
  // la lettura è locale altrimenti è remota
  //-----------------------------------------------
  if(break_Com[COM3]==false){
     // SEMILAVORATI
     if(FormCercaCod->TipoPRD==0){
        if(RGOrdine->ItemIndex==0) err = FormSQLDB->lista_codPRD_SQL("NUM", PRD, &NumRec, all_mess);
        if(RGOrdine->ItemIndex==1) err = FormSQLDB->lista_codPRD_SQL("COD", PRD, &NumRec, all_mess);
        if(RGOrdine->ItemIndex==2) err = FormSQLDB->lista_codPRD_SQL("DES", PRD, &NumRec, all_mess);
        // compilazione lista
        for(i=0; i<NumRec; i++){
           if(err) break;
           j++;
           CercaGrid->Cells[0][j] = PRD[i].id_record;
           CercaGrid->Cells[1][j] = PRD[i].codice;
           CercaGrid->Cells[2][j] = PRD[i].descrizione;
        }
        CercaGrid->RowCount = j+1;
        Edit2->Text = CercaPRD.descrizione;
        Edit1->Text = CercaPRD.codice;
        Edit1->SetFocus();
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormCercaCod::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if( Shift.Contains(ssAlt) && Key==VK_F4 ) return;
  switch (Key) {
     case VK_ESCAPE: Close();    break;
     default: break;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormCercaCod::Edit1Change(TObject *Sender)
{
  int i, num_ch;
  char buffer[51]="";
  char stringa[51]="";
  char stringa1[51]="";

  if(EditDes) return;

  strcpy(stringa, Edit1->Text.c_str());
  //------------------------------------------
  // Se non è stato selezionato nessun codice
  // --> Skip!!!
  //------------------------------------------
  if(Edit1->Text.IsEmpty()){
     CercaGrid->Row=0;
     return;
  }
  //------------------------------------------
  // Ricerca della riga nella griglia
  //------------------------------------------
  num_ch = strlen(stringa);
  for(i=0; i<CercaGrid->RowCount; i++){
     strcpy(buffer, CercaGrid->Cells[1][i].c_str());
     //--------------------------------------------------------
     // Recupero di una parte di stringa lunga come la stringa
     // di ricerca.
     //--------------------------------------------------------
     Copy(buffer, 1, num_ch, stringa1);
     //--------------------------------------------------------
     // Controllo se la stringa recuperata dalla lista è uguale
     // alla stringa di ricerca.
     //--------------------------------------------------------
     if(strcmp(stringa, stringa1)!=0) continue;
     //--------------------------------------------------------
     // Indicizzazione della griglia
     //--------------------------------------------------------
     CercaGrid->Row = i;
     Edit2->Text = CercaGrid->Cells[2][i];
     break;
  }
  return;

}
//---------------------------------------------------------------------------
void __fastcall TFormCercaCod::Edit2Change(TObject *Sender)
{

  int i, num_ch;
  char buffer[51]="";
  char stringa[51]="";
  char stringa1[51]="";


  if(EditCod) return;

  strcpy(stringa, Edit2->Text.c_str());
  //------------------------------------------
  // Se non è stato selezionato nessun codice
  // --> Skip!!!
  //------------------------------------------
  if(Edit2->Text.IsEmpty()){
     CercaGrid->Row=0;
     return;
  }
  //------------------------------------------
  // Ricerca della riga nella griglia
  //------------------------------------------
  num_ch = strlen(stringa);
  for(i=0; i<CercaGrid->RowCount; i++){
     strcpy(buffer, CercaGrid->Cells[2][i].c_str());
     //--------------------------------------------------------
     // Recupero di una parte di stringa lunga come la stringa
     // di ricerca.
     //--------------------------------------------------------
     Copy(buffer, 1, num_ch, stringa1);
     //--------------------------------------------------------
     // Controllo se la stringa recuperata dalla lista è uguale
     // alla stringa di ricerca.
     //--------------------------------------------------------
     if(strcmp(stringa, stringa1)!=0) continue;
     //--------------------------------------------------------
     // Indicizzazione della griglia
     //--------------------------------------------------------
     CercaGrid->Row = i;
     Edit1->Text = CercaGrid->Cells[1][i];
     break;
  }
  return;

}
//---------------------------------------------------------------------------

void __fastcall TFormCercaCod::CercaGridClick(TObject *Sender)
{
  int Riga = CercaGrid->Row;

  if(EditCod || EditDes) return;
  EditCod=true;                             // scorrimento della griglia per codice
  Edit1->Text = CercaGrid->Cells[1][Riga];
  Edit2->Text = CercaGrid->Cells[2][Riga];
  EditCod=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormCercaCod::Edit1Enter(TObject *Sender)
{
  EditCod=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormCercaCod::Edit1Exit(TObject *Sender)
{
  EditCod=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormCercaCod::Edit2Exit(TObject *Sender)
{
  EditDes=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormCercaCod::Edit2Enter(TObject *Sender)
{
  EditDes=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormCercaCod::BtnOkClick(TObject *Sender)
{
  int  id, err=0;
  char all_mess[101]="";
  int  index = CercaGrid->Row;

  id = (int)(atoi(CercaGrid->Cells[0][index].c_str()));
  //--------------------------------------------
  // SELEZIONE CODICE SEMILAVORATO
  //--------------------------------------------
  if(FormCercaCod->TipoPRD==0){
     memset(&CercaPRD, 0, sizeof(DATI_PRD));
     if(id!=0) err = FormSQLDB->cerca_codPRD_SQL("", id, "NUM", &CercaPRD, all_mess);
  }
  if(err) ShowMessage(all_mess);
}
//---------------------------------------------------------------------------
void __fastcall TFormCercaCod::CercaGridDblClick(TObject *Sender)
{
  int  id, err=0;
  char all_mess[101]="";
  int  index = CercaGrid->Row;

  id = (int)(atoi(CercaGrid->Cells[0][index].c_str()));
  //--------------------------------------------
  // SELEZIONE CODICE SEMILAVORATO
  //--------------------------------------------
  if(FormCercaCod->TipoPRD==0){
     memset(&CercaPRD, 0, sizeof(DATI_PRD));
     if(id!=0) err = FormSQLDB->cerca_codPRD_SQL("", id, "NUM", &CercaPRD, all_mess);
  }
  if(err ) ShowMessage(all_mess);
  if(!err) FormCercaCod->ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TFormCercaCod::RGOrdineClick(TObject *Sender)
{
  int  err, i, j;
  int  NumRec;
  char all_mess[101]="";
  struct DATI_PRD PRD[MAXCODICI+1];

  err=0;
  NumRec=0;
  EditCod=true;
  EditDes=false;
  memset(&PRD, 0, sizeof(PRD));
  //-----------------------------------------------
  // se la comunicazione con DBTECNO è disattivata
  // la lettura è locale altrimenti è remota
  //-----------------------------------------------
  j=0;
  CercaGrid->Cells[0][j] = "0";
  CercaGrid->Cells[1][j] = "";
  CercaGrid->Cells[2][j] = "";
  if(break_Com[COM3]==false){
     // SEMILAVORATI
     if(FormCercaCod->TipoPRD==0){
        if(RGOrdine->ItemIndex==0) err = FormSQLDB->lista_codPRD_SQL("NUM", PRD, &NumRec, all_mess);
        if(RGOrdine->ItemIndex==1) err = FormSQLDB->lista_codPRD_SQL("COD", PRD, &NumRec, all_mess);
        if(RGOrdine->ItemIndex==2) err = FormSQLDB->lista_codPRD_SQL("DES", PRD, &NumRec, all_mess);
        // compilazione lista
        for(i=0; i<NumRec; i++){
           if(err) break;
           j++;
           CercaGrid->Cells[0][j] = PRD[i].id_record;
           CercaGrid->Cells[1][j] = PRD[i].codice;
           CercaGrid->Cells[2][j] = PRD[i].descrizione;
        }
        CercaGrid->RowCount = j+1;
     }
  }
}
//---------------------------------------------------------------------------

