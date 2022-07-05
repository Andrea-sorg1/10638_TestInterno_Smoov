//---------------------------------------------------------------------------
#ifndef MainResetOpcH
#define MainResetOpcH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <DBTables.hpp>
#include <vcl\inifiles.hpp>

//---------------------------------------------------------------------------
//#include "Sistema.h"
#include <DB.hpp>
#include <ImgList.hpp>
#include "trayicon.h"
#include <Dialogs.hpp>
#include "WSocket.hpp"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
  TImageList *ImgLst1;
  TMainMenu *MainMenu;
  TPopupMenu *PopupMenu1;
  TMenuItem *MShow;
  TMenuItem *N4;
  TMenuItem *MHide;
  TMenuItem *N5;
  TMenuItem *MClose;
  TStatusBar *SBarDB;
  TTimer *TimerCiclo;
  TImageList *ImgLst2;
  TTrayIcon *CoolTrayIconDB;
  TOpenDialog *OpenDialogServer;
  TButton *PathOpcClient;
  TLabel *Label1;
  TLabel *Label2;
  TEdit *Edit5;
  TLabel *Label5;
  TWSocket *WSocket0;
  TWSocket *WSocket1;
  TWSocket *WSocket2;
  TWSocket *WSocket3;
  TWSocket *WSocket4;
  TWSocket *WSocket5;
  TWSocket *WSocket6;
  TWSocket *WSocket7;
  TWSocket *WSocket8;
  TWSocket *WSocket9;
  TWSocket *WSocket10;
  TWSocket *WSocket11;
  TWSocket *WSocket12;
  TWSocket *WSocket13;
  TWSocket *WSocket14;
  TWSocket *WSocket15;
  TWSocket *WSocket16;
  TWSocket *WSocket17;
  TWSocket *WSocket18;
  TWSocket *WSocket19;
  TWSocket *WSocket20;
  TWSocket *WSocket21;
  TWSocket *WSocket22;
  TWSocket *WSocket23;
  TWSocket *WSocket24;
  TWSocket *WSocket25;
  TWSocket *WSocket26;
  TWSocket *WSocket27;
  TWSocket *WSocket28;
  TWSocket *WSocket29;
  TWSocket *WSocket30;
  TWSocket *WSocket31;
  TWSocket *WSocket32;
  TWSocket *WSocket33;
  TWSocket *WSocket34;
  TWSocket *WSocket35;
  TWSocket *WSocket36;
  TWSocket *WSocket37;
  TWSocket *WSocket38;
  TWSocket *WSocket39;
  TWSocket *WSocket40;
  TWSocket *WSocket41;
  TWSocket *WSocket42;
  TWSocket *WSocket43;
  TWSocket *WSocket44;
  TWSocket *WSocket45;
  TWSocket *WSocket46;
  TWSocket *WSocket47;
  TWSocket *WSocket48;
  TWSocket *WSocket49;
  TWSocket *WSocket50;
  TWSocket *WSocket51;
  TButton *BtnResetConsensi;
  void __fastcall MExitClick(TObject *Sender);
  void __fastcall MTileClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall MHideClick(TObject *Sender);
  void __fastcall MShowClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);

  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall MMinimizedClick(TObject *Sender);


  void __fastcall TimerCicloTimer(TObject *Sender);
  void __fastcall PathOpcClientClick(TObject *Sender);
  void __fastcall BtnResetConsensiClick(TObject *Sender);
  
private:	// User declarations

  bool PrimaScansione;

public:		// User declarations

  bool ComandoIntercettato;
  bool ErroreGenericoServer;
  bool CancellazioneAvvenuta;
  bool CompattazioneInCorso;
  int  TFormMain::ReadWrite(char* all_mess);

  bool LetturaCiclica;

  __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
