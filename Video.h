//---------------------------------------------------------------------------
#ifndef VideoH
#define VideoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <MPlayer.hpp>
//---------------------------------------------------------------------------
class TFormVideo : public TForm
{
__published:	// IDE-managed Components
  TPanel *VideoPanel;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TLabel *Label4;
  TBevel *Bevel1;
  TPanel *PanelLogo;
  TImage *ImageLogo;
  TMediaPlayer *PresentaMediaPlayer;
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormCreate(TObject *Sender);
  
  
private:	// User declarations
public:		// User declarations
  __fastcall TFormVideo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormVideo *FormVideo;
//---------------------------------------------------------------------------
#endif
