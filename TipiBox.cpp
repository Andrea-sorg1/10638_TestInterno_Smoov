//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TipiBox.h"
#include "Tecnofer.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTipiBox *FormTipiBox;
//---------------------------------------------------------------------------
__fastcall TFormTipiBox::TFormTipiBox(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTipiBox::FormActivate(TObject *Sender)
{
  int i=0;
  //------------------------------------------
  // Gruppo di appartenenza
  //------------------------------------------
  CBTipoBox->Clear();
  for(i=0; i<MAXTIPOBOXIMP; i++){
     if(strcmp(M.TipiBox[i].Nome, "")==0) break;
     CBTipoBox->Items->Add(M.TipiBox[i].Nome);
  }
  // ----------------------------------------
  // Punto al primo elemento della combo box
  // ----------------------------------------
  CBTipoBox->ItemIndex=0;
  // --- imposta i messaggi degli oggetti ---
  Messaggi();
  // ----------------------------------------
  // Aggiorno i dati delle caselle
  // ----------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
// +------------------------------------------+
// | Imposta i messaggi abbinati agli oggetti |
// +------------------------------------------+
void __fastcall TFormTipiBox::Messaggi()
{
  btnSalva->Caption     = MESS[337];
  btnEsci->Caption      = MESS[171];

  FormTipiBox->Caption  = MESS[731];
  lblTipoBox->Caption   = MESS[732];

  gbDimensioni->Caption = MESS[733];
  lblLung->Caption      = MESS[734];
  lblLarg->Caption      = MESS[735];
  lblAltezza->Caption   = MESS[736];

  gbConfig->Caption     = MESS[737];
  lblOffset->Caption    = MESS[738];
  lblDistanza->Caption  = MESS[739];
  lblArrotond->Caption  = MESS[740];

}
//---------------------------------------------------------------------------
void __fastcall TFormTipiBox::FormKeyDown(TObject *Sender, WORD &Key,  TShiftState Shift)
{
  switch (Key){case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------
void __fastcall TFormTipiBox::SetCaselle()
{
  int index=0;
  char buffer[11];

  //-------------------------------------
  // Recupero indice struttura
  //-------------------------------------
  index = CBTipoBox->ItemIndex;
  //-------------------------------------
  // Aggiornamento caselle di editor
  //-------------------------------------
  sprintf(buffer, "%03d", M.TipiBox[index].Lung);
  MELung->Text=buffer;
  sprintf(buffer, "%03d", M.TipiBox[index].Larg);
  MELarg->Text=buffer;
  sprintf(buffer, "%03d", M.TipiBox[index].Altez);
  MEAltez->Text=buffer;
  sprintf(buffer, "%02d", M.TipiBox[index].offset_tgv_da_nodo_mag);
  MEOffset->Text=buffer;
  sprintf(buffer, "%02d", M.TipiBox[index].dist_box_in_magaz);
  MEDist->Text=buffer;
  sprintf(buffer,"%03.2f",M.TipiBox[index].eccesso_count_pos_mag);
  MEArr->Text=buffer;
}
//---------------------------------------------------------------------------
void __fastcall TFormTipiBox::GetCaselle()
{
  int index=0;

  //-------------------------------------
  // Recupero indice struttura
  //-------------------------------------
  index = CBTipoBox->ItemIndex;
  //-------------------------------------
  // Aggiornamento struttura
  //-------------------------------------
  M.TipiBox[index].Lung                   = atoi(MELung->Text.c_str());
  M.TipiBox[index].Larg                   = atoi(MELarg->Text.c_str());
  M.TipiBox[index].Altez                  = atoi(MEAltez->Text.c_str());
  M.TipiBox[index].dist_box_in_magaz      = atoi(MEDist->Text.c_str());
  M.TipiBox[index].eccesso_count_pos_mag  = atof(MEArr->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormTipiBox::CBTipoBoxChange(TObject *Sender)
{
  //----------------------------
  // Aggiorno finestra
  //----------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------
void __fastcall TFormTipiBox::btnSalvaClick(TObject *Sender)
{
  int  i, j, k, err=0;
  double a, b;
  short int new_npt;
  short int lato_box=LUNG_CEST;
  char buffer[101]="";

  //----------------------------------------------------------------
  // Aggiorno struttura
  //----------------------------------------------------------------
  GetCaselle();

  //----------------------------------------------------------------
  // GESTIONE DINAMICA POSIZIONI IN MAGAZZINO
  //
  // Ricalcolo le posizioni ammesse in magazzino nella superficie
  // di deposito ammessa
  //----------------------------------------------------------------
  if(GestioneDinamicaMagazzini==1){
     //---------------------------------------------------------
     // Recupero il lato box su cui eseguire il calcolo
     //---------------------------------------------------------
     for(i=1; i<=TOTMAG; i++){
        if(M.m[i]->Num<1     ) continue;
        if(M.m[i]->Num>TOTMAG) continue;
        lato_box = M.TipiBox[M.m[i]->TipoBox].Lung;
        //---------------------------------------------
        // Calcolo postazioni a terra
        //
        // Arrotondo per eccesso quando i decimali
        // superano 0.6 pallet
        //---------------------------------------------
        //M.EditMag.Npt = (short int)(M.m[i]->Profondita/(lato_box+DIST_CEST_IN_MAG))+1;
        a = (double)(M.m[i]->Profondita)/(double)(lato_box+M.TipiBox[M.m[i]->TipoBox].dist_box_in_magaz);
        b = (double)(a) - (int)(a);
        if(b>M.TipiBox[M.m[i]->TipoBox].eccesso_count_pos_mag) a = (a - b) + 1;      // tolgo i decimali al valore calcolato e aggingo "1".
        new_npt = (short int)(a)+1;     // calcolo effettivo palette a terra
        //------------------------------------------------
        // Aggiorno struttura magazzini modificati
        //------------------------------------------------
        if(M.m[i]->Npt!=new_npt){
           M.m[i]->Npt = new_npt;
           //------------------------------------------------
           // Svuotamento posizioni inesisenti
           //------------------------------------------------
           for(j=new_npt+1; j<=MAXTERRAMAG; j++){
              for(k=0; k<MAXPIANIMAG; k++){
                 M.m[i]->Pos[j].Presenza[k] = false;
                 M.m[i]->Pos[j].QuotaH[k]   = 0;
                 M.m[i]->Pos[j].QuotaP[k]   = 0;
                 memset(&M.m[i]->Pos[j].Box[k], 0, sizeof(BOX));
              }
           }
           //------------------------------------------------
           // Salvataggio su file
           //------------------------------------------------
           err = M.save_record_mag( M.m[i]->Num, buffer);
           if( err ){
              Application->MessageBox( buffer, MESS[100], MB_OK | MB_ICONEXCLAMATION);
              break;
           }              
        }
     }
  }
  //----------------------------------------------------------------
  // Salvataggio su file
  //----------------------------------------------------------------
  if(!err){
     err = M.save_file_tipibox(buffer);
     if( err ) ShowMessage(buffer);
  }
  //----------------------------------------------------------------
  // Aggiorno finestra
  //----------------------------------------------------------------
  SetCaselle();
}
//---------------------------------------------------------------------------


