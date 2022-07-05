//---------------------------------------------------------------------------
#ifndef UdpServer1H
#define UdpServer1H
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <NMUDP.hpp>
#include <ExtCtrls.hpp>
#include "WSocket.hpp"
#include "Ping.hpp"

#ifndef MAINFILE
    #define TIPO_VARIABILE extern
#else
    #define TIPO_VARIABILE
#endif

TIPO_VARIABILE int NumCom;

//---------------------------------------------------------------------------
class TFormServerUDP : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *BoxInterfaccia;
  TComboBox *ListaPLC;
  TStaticText *TextId;
  TRadioGroup *BoxComunicazione;
  TGroupBox *BoxHostLink;
  TLabel *Label4;
  TGroupBox *BoxCOM;
  TComboBox *ListaCOM;
  TButton *BtnSetCom;
  TGroupBox *BoxIdPlc;
  TEdit *IndicePlcEdit;
  TStaticText *TextParCom;
  TEdit *HOSTTimeOutEdit;
  TGroupBox *BoxFinsProtocol;
  TLabel *Label8;
  TLabel *Label2;
  TLabel *Label1;
  TLabel *Label5;
  TLabel *Label10;
  TLabel *Label9;
  TLabel *Label13;
  TLabel *Label11;
  TLabel *Label14;
  TLabel *Label12;
  TLabel *Label3;
  TEdit *LocalIpEdit;
  TEdit *RemoteIpEdit;
  TEdit *PortEdit;
  TEdit *RemotePortEdit;
  TEdit *FinsSnaEdit;
  TEdit *FinsDnaEdit;
  TEdit *FinsSa1Edit;
  TEdit *FinsDa1Edit;
  TEdit *FinsSa2Edit;
  TEdit *FinsDa2Edit;
  TEdit *UDPTimeOutEdit;
  TGroupBox *BoxTestRapido;
  TLabel *Label6;
  TLabel *Label7;
  TLabel *DataAvailableLabel;
  TEdit *NrDatoIniEdit;
  TEdit *NrDatiEdit;
  TButton *SendButton;
  TListBox *ListBox1;
  TRadioGroup *BoxCmd;
  TButton *BtnSalva;
  TButton *BtnEsci;
  TGroupBox *BoxEthRS232;
  TLabel *Label15;
  TLabel *Label16;
  TLabel *Label17;
  TLabel *Label18;
  TEdit *TCPTimeOutEdit;
  TEdit *EditPlcTCP;
  TEdit *EditPortTCP;
  TEdit *EditIpTCP;
  TWSocket *UdpWSocket1;
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
  TWSocket *WSocket52;
  TWSocket *WSocket53;
  TWSocket *WSocket54;
  TWSocket *WSocket55;
  TWSocket *WSocket56;
  TWSocket *WSocket57;
  TWSocket *WSocket58;
  TWSocket *WSocket59;
  TWSocket *WSocket60;
  TWSocket *WSocket61;
  TWSocket *WSocket62;
  TWSocket *WSocket63;
  TWSocket *WSocket64;
  TWSocket *WSocket65;
  TWSocket *WSocket66;
  TWSocket *WSocket67;
  TWSocket *WSocket68;
  TWSocket *WSocket69;
  TWSocket *WSocket70;
  TWSocket *WSocket71;
  TWSocket *WSocket72;
  TWSocket *WSocket73;
  TWSocket *WSocket74;
  TWSocket *WSocket75;
  TWSocket *WSocket76;
  TWSocket *WSocket77;
  TWSocket *WSocket78;
  TWSocket *WSocket79;
  TWSocket *WSocket80;
  TWSocket *WSocket81;
  TWSocket *WSocket82;
  TWSocket *WSocket83;
  TWSocket *WSocket84;
  TWSocket *WSocket85;
  TWSocket *WSocket86;
  TWSocket *WSocket87;
  TWSocket *WSocket88;
  TWSocket *WSocket89;
  TWSocket *WSocket90;
  TWSocket *WSocket91;
  TWSocket *WSocket92;
  TWSocket *WSocket93;
  TWSocket *WSocket94;
  TWSocket *WSocket95;
  TWSocket *WSocket96;
  TWSocket *WSocket97;
  TWSocket *WSocket98;
  TWSocket *WSocket99;
  TWSocket *WSocket100;

  TLabel *Label19;
  TEdit *EditSA1Tcp;
  TPing *Ping1;
  TButton *BtnPing;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall BtnSalvaClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall ListaPLCClick(TObject *Sender);
  void __fastcall BoxComunicazioneClick(TObject *Sender);
  void __fastcall BtnEsciClick(TObject *Sender);

  void __fastcall BtnSetComClick(TObject *Sender);
  void __fastcall ListaCOMClick(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);



  void __fastcall SendButtonClick(TObject *Sender);
  void __fastcall BtnPingClick(TObject *Sender);
private:	// User declarations

    AnsiString   FIniFileName;
    AnsiString   FSectionName;
    AnsiString   FKeyName;
    AnsiString   FPath;

    int __fastcall GetCaselle();
    int __fastcall SetCaselle();

public:		// User declarations

    TInAddr      FServerAddr;
    AnsiString   LocalIPAddr;

    //--------------------------------
    // funzioni publiche
    //--------------------------------
    void IPToNode(AnsiString IPAddr, unsigned char *Node);

    __fastcall TFormServerUDP(TComponent* Owner);
};

int AlgoPing( char* IpAddress, int TimeOut, int* MsecRisp, char* all_mess );

//---------------------------------------------------------------------------
extern TFormServerUDP *FormServerUDP;
//---------------------------------------------------------------------------
#endif
