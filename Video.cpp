//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>

#pragma hdrstop

#include "Video.h"
#include "Tecnofer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormVideo *FormVideo;
//---------------------------------------------------------------------------
__fastcall TFormVideo::TFormVideo(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormVideo::FormCreate(TObject *Sender)
{
  PanelLogo->Visible=false;
  //----------------------------------------------
  // Recupero la locazione del file da ripordurre
  //----------------------------------------------
//PresentaMediaPlayer->FileName = PathProject + "Avi\\porco.avi";
  //----------------------------------------------
  // Recupero il LOGO DEL'IMPIANTO
  //----------------------------------------------
  ImageLogo->Picture->LoadFromFile("Grafica\\LogoImpianto.bmp");
}
//---------------------------------------------------------------------------
void __fastcall TFormVideo::FormActivate(TObject *Sender)
{
  AnsiString NomeFile;

  NomeFile = PathProject + "Avi\\porco.avi";
  //----------------------------------------------
  // Verifico se il file esiste
  //----------------------------------------------
  if(access(NomeFile.c_str(), 0)!=0){
     PanelLogo->Visible=true;
     return;
  }
  //----------------------------------------------
  // Apertura dell'oggetto di ripoduzione
  //----------------------------------------------
  PresentaMediaPlayer->Open();
  //----------------------------------------------
  // Start riproduzione
  //----------------------------------------------
  PresentaMediaPlayer->Play();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideo::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch (Key){ case VK_ESCAPE: Close(); break;}
}
//---------------------------------------------------------------------------
void __fastcall TFormVideo::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  AnsiString NomeFile;

  NomeFile = PathProject + "Avi\\porco.avi";
  //----------------------------------------------
  // Verifico se il file esiste
  //----------------------------------------------
  if(access(NomeFile.c_str(), 0)!=0) return;
  //---------------------------------------
  // Stop riproduzione
  //---------------------------------------
  PresentaMediaPlayer->Stop();
  //---------------------------------------
  // Apertura dell'oggetto di ripoduzione
  //---------------------------------------
  PresentaMediaPlayer->Close();
}
//---------------------------------------------------------------------------


