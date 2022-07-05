//---------------------------------------------------------------------------

#ifndef AsserviH
#define AsserviH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormAsservi : public TForm
{
__published:	// IDE-managed Components
  TBitBtn *BtnOK;
  TGroupBox *GBMac;
  TLabel *Label11;
  TLabel *Label12;
  TLabel *Label13;
  TLabel *Label14;
  TLabel *Label15;
  TComboBox *EditComboMac1;
  TComboBox *EditComboMac2;
  TComboBox *EditComboMac3;
  TComboBox *EditComboMac4;
  TComboBox *EditComboMac5;
  TComboBox *EditComboMac6;
  TCheckBox *CBing1;
  TCheckBox *CBout1;
  TCheckBox *CBout2;
  TCheckBox *CBing2;
  TCheckBox *CBout3;
  TCheckBox *CBing3;
  TCheckBox *CBout4;
  TCheckBox *CBing4;
  TCheckBox *CBout5;
  TCheckBox *CBing5;
  TCheckBox *CBout6;
  TCheckBox *CBing6;
  TBevel *Bevel1;
  TBevel *Bevel2;
  TBevel *Bevel3;
  TBevel *Bevel4;
  TBevel *Bevel5;
  TBevel *Bevel6;
  TComboBox *EditComboMac7;
  TCheckBox *CBout7;
  TCheckBox *CBing7;
  TBevel *Bevel7;
  TComboBox *EditComboMac8;
  TCheckBox *CBout8;
  TCheckBox *CBing8;
  TLabel *Label16;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TComboBox *EditComboMac9;
  TCheckBox *CBout9;
  TCheckBox *CBing9;
  TBevel *Bevel8;
  TLabel *Label4;
  TComboBox *EditComboMac10;
  TCheckBox *CBout10;
  TCheckBox *CBing10;
  TBevel *Bevel9;
  TLabel *Label5;
  TComboBox *EditComboMac11;
  TCheckBox *CBout11;
  TCheckBox *CBing11;
  TBevel *Bevel10;
  TLabel *Label6;
  TComboBox *EditComboMac12;
  TCheckBox *CBout12;
  TCheckBox *CBing12;
  TBevel *Bevel11;
  TLabel *Label7;
  TComboBox *EditComboMac13;
  TCheckBox *CBout13;
  TCheckBox *CBing13;
  TBevel *Bevel12;
  TLabel *Label8;
  TComboBox *EditComboMac14;
  TCheckBox *CBout14;
  TCheckBox *CBing14;
  TBevel *Bevel13;
  TLabel *Label9;
  TComboBox *EditComboMac15;
  TCheckBox *CBout15;
  TCheckBox *CBing15;
  TBevel *Bevel14;
  TLabel *Label10;
  TComboBox *EditComboMac16;
  TCheckBox *CBout16;
  TCheckBox *CBing16;
  TBevel *Bevel15;
  TLabel *Label17;
  TComboBox *EditComboMac17;
  TCheckBox *CBout17;
  TCheckBox *CBing17;
  TBevel *Bevel16;
  TLabel *Label18;
  TComboBox *EditComboMac18;
  TCheckBox *CBout18;
  TCheckBox *CBing18;
  TBevel *Bevel17;
  TLabel *Label19;
  TComboBox *EditComboMac19;
  TCheckBox *CBout19;
  TCheckBox *CBing19;
  TBevel *Bevel18;
  TLabel *Label20;
  TComboBox *EditComboMac20;
  TCheckBox *CBout20;
  TCheckBox *CBing20;
  TBevel *Bevel19;
  TGroupBox *GBBaie;
  TLabel *Label21;
  TLabel *Label22;
  TLabel *Label23;
  TLabel *Label24;
  TLabel *Label25;
  TBevel *Bevel20;
  TBevel *Bevel21;
  TBevel *Bevel22;
  TBevel *Bevel23;
  TBevel *Bevel24;
  TBevel *Bevel25;
  TBevel *Bevel26;
  TLabel *Label26;
  TLabel *Label27;
  TLabel *Label28;
  TLabel *Label29;
  TBevel *Bevel27;
  TLabel *Label30;
  TBevel *Bevel28;
  TLabel *Label31;
  TBevel *Bevel29;
  TLabel *Label32;
  TBevel *Bevel30;
  TLabel *Label33;
  TBevel *Bevel31;
  TLabel *Label34;
  TBevel *Bevel32;
  TLabel *Label35;
  TBevel *Bevel33;
  TLabel *Label36;
  TBevel *Bevel34;
  TLabel *Label37;
  TBevel *Bevel35;
  TLabel *Label38;
  TBevel *Bevel36;
  TLabel *Label39;
  TBevel *Bevel37;
  TLabel *Label40;
  TBevel *Bevel38;
  TComboBox *EditComboBaie1;
  TComboBox *EditComboBaie2;
  TComboBox *EditComboBaie3;
  TComboBox *EditComboBaie4;
  TComboBox *EditComboBaie5;
  TComboBox *EditComboBaie6;
  TComboBox *EditComboBaie7;
  TComboBox *EditComboBaie8;
  TComboBox *EditComboBaie9;
  TComboBox *EditComboBaie10;
  TComboBox *EditComboBaie11;
  TComboBox *EditComboBaie12;
  TComboBox *EditComboBaie13;
  TComboBox *EditComboBaie14;
  TComboBox *EditComboBaie15;
  TComboBox *EditComboBaie16;
  TComboBox *EditComboBaie17;
  TComboBox *EditComboBaie18;
  TComboBox *EditComboBaie19;
  TComboBox *EditComboBaie20;
  TLabel *Label41;
  TComboBox *EditComboBaie21;
  TBevel *Bevel39;
  TComboBox *EditComboBaie22;
  TLabel *Label42;
  TBevel *Bevel40;
  TLabel *Label43;
  TComboBox *EditComboBaie23;
  TLabel *Label44;
  TComboBox *EditComboBaie24;
  TBevel *Bevel41;
  TLabel *Label45;
  TComboBox *EditComboBaie25;
  TBevel *Bevel42;
  TLabel *Label46;
  TComboBox *EditComboBaie26;
  TLabel *Label47;
  TBevel *Bevel43;
  TComboBox *EditComboBaie27;
  TBevel *Bevel44;
  TLabel *Label48;
  TComboBox *EditComboBaie28;
  TBevel *Bevel45;
  TLabel *Label49;
  TComboBox *EditComboBaie29;
  TBevel *Bevel46;
  TLabel *Label50;
  TComboBox *EditComboBaie30;
  TBevel *Bevel47;
  TLabel *Label51;
  TComboBox *EditComboBaie31;
  TBevel *Bevel48;
  TLabel *Label52;
  TComboBox *EditComboBaie32;
  TBevel *Bevel49;
  TLabel *Label53;
  TComboBox *EditComboBaie33;
  TBevel *Bevel50;
  TLabel *Label54;
  TComboBox *EditComboBaie34;
  TBevel *Bevel51;
  TLabel *Label55;
  TComboBox *EditComboBaie35;
  TBevel *Bevel52;
  TLabel *Label56;
  TComboBox *EditComboBaie36;
  TLabel *Label57;
  TComboBox *EditComboBaie37;
  TBevel *Bevel53;
  TBevel *Bevel54;
  TLabel *Label58;
  TComboBox *EditComboBaie38;
  TLabel *Label59;
  TBevel *Bevel55;
  TComboBox *EditComboBaie39;
  TBevel *Bevel56;
  TLabel *Label60;
  TComboBox *EditComboBaie40;
  TBevel *Bevel57;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall BtnOKClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations

  void __fastcall Gruppi();
  void __fastcall Messaggi();
  void __fastcall SetCaselle();
  void __fastcall GetCaselle();

  TComboBox *CBMac[MAXCHIAMATE+1];
  TCheckBox *CBing[MAXCHIAMATE+1];
  TCheckBox *CBout[MAXCHIAMATE+1];
  TComboBox *CBBai[41];

public:		// User declarations

  int NumTgv;

  __fastcall TFormAsservi(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAsservi *FormAsservi;
//---------------------------------------------------------------------------
#endif
