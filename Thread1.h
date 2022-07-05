//---------------------------------------------------------------------------
#ifndef Thread1H
#define Thread1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TMyThread1 : public TThread
{
private:
protected:

  void __fastcall Execute();
  void __fastcall FunzioneSincrona();
  
public:

  int CountThread;
  long int TimeLastThread;

    __fastcall TMyThread1();
};
//---------------------------------------------------------------------------
#endif
