//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Main.cpp", FormMain);
USEFORM("Layout.cpp", FormLayout);
USEFORM("Stato.cpp", FormStato);
USEFORM("Password.cpp", FormPassword);
USEFORM("Messagg.cpp", FormMessaggi);
USEFORM("UdpServer1.cpp", FormServerUDP);
USEFORM("InitCom.cpp", FormSetCom);
USEFORM("SQLDB.cpp", FormSQLDB);
USEFORM("Agvwin.cpp", FormTgv);
USEFORM("Asservi.cpp", FormAsservi);
USEFORM("Mapping.cpp", FormMappa);
USEFORM("Nodowin.cpp", FormNodi);
USEFORM("Magwin.cpp", FormMagaz);
USEFORM("Baiwin.cpp", FormBaie);
USEFORM("Boxwin.cpp", FormBox);
USEFORM("Listagen.cpp", FormListagen);
USEFORM("ListaCar.cpp", FormListaCar);
USEFORM("ElCar2.cpp", FormElBox);
USEFORM("AbbPLC.cpp", FormAbbPlc);
USEFORM("TipiBox.cpp", FormTipiBox);
USEFORM("PorteImp.cpp", FormPorteImp);
USEFORM("SemiAuto.cpp", FormSemiAuto);
USEFORM("TestP.cpp", FormTest);
USEFORM("TestPlc1.cpp", FormTestPlc);
USEFORM("TestPlc2.cpp", FormTestPlc2);
USEFORM("TestPlc3.cpp", FormTestPlc3);
USEFORM("StatTgv.cpp", FormStatTgv);
USEFORM("Input.cpp", FormInput);
USEFORM("Video.cpp", FormVideo);
USEFORM("Visfile.cpp", FormVisualizzaFile);
USEFORM("XYNodi.cpp", FormXYNodi);
USEFORM("SrvTcpAndroid.cpp", FormSrvTcpAndroid);
USEFORM("DisRiflettori.cpp", FormDisRiflettori);
USEFORM("StatoBaiewin.cpp", FormStatoBaie);
USEFORM("StatoMagwin.cpp", FormStatoMagaz);
USEFORM("Batwin.cpp", FormCambioBatteria);
USEFORM("GrpDest.cpp", FormGrpDest);
USEFORM("GruppiBa.cpp", FormGruppiBa);
USEFORM("StatTgvEstesa.cpp", FormStatTgvEstesa);
USEFORM("DBGridGen.cpp", FormDBGridGen);
USEFORM("Macwin.cpp", FormMacchine);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  HANDLE MuteKing;
  LPCTSTR Nome="Smoov";
  DWORD  Hurricane;

  //--------------------------------------------------------------
  // Crea un oggetto MUTEX
  //---------------------------------------------------------------
  MuteKing=CreateMutex(NULL, true, Nome);
  //---------------------------------------------------------------
  // Se la funzione di creazione di oggeto MUTEX ritorna un valore
  // diverso da "0" si è verificato un ERRORE.
  //---------------------------------------------------------------
  if(MuteKing!=NULL){
     //------------------------------------------------------------
     // Se l'ERRORE è dovuto al fatto che l'oggetto è già stato
     // creaqto ---> Skip!!!
     //------------------------------------------------------------
     Hurricane=GetLastError();
     if(Hurricane==ERROR_ALREADY_EXISTS) return 0;
  }
  try
  {
     Application->Initialize();
     Application->Title = "Software Icam/TecnoFerrari";
     Application->CreateForm(__classid(TFormMain), &FormMain);
     Application->CreateForm(__classid(TFormLayout), &FormLayout);
     Application->CreateForm(__classid(TFormStato), &FormStato);
     Application->CreateForm(__classid(TFormPassword), &FormPassword);
     Application->CreateForm(__classid(TFormMessaggi), &FormMessaggi);
     Application->CreateForm(__classid(TFormServerUDP), &FormServerUDP);
     Application->CreateForm(__classid(TFormSetCom), &FormSetCom);
     Application->CreateForm(__classid(TFormTgv), &FormTgv);
     Application->CreateForm(__classid(TFormInput), &FormInput);
     Application->CreateForm(__classid(TFormListagen), &FormListagen);
     Application->CreateForm(__classid(TFormMappa), &FormMappa);
     Application->CreateForm(__classid(TFormNodi), &FormNodi);
     Application->CreateForm(__classid(TFormPorteImp), &FormPorteImp);
     Application->CreateForm(__classid(TFormSemiAuto), &FormSemiAuto);
     Application->CreateForm(__classid(TFormStatTgv), &FormStatTgv);
     Application->CreateForm(__classid(TFormTest), &FormTest);
     Application->CreateForm(__classid(TFormTestPlc), &FormTestPlc);
     Application->CreateForm(__classid(TFormTestPlc2), &FormTestPlc2);
     Application->CreateForm(__classid(TFormTestPlc3), &FormTestPlc3);
     Application->CreateForm(__classid(TFormVideo), &FormVideo);
     Application->CreateForm(__classid(TFormVisualizzaFile), &FormVisualizzaFile);
     Application->CreateForm(__classid(TFormXYNodi), &FormXYNodi);
     Application->CreateForm(__classid(TFormAsservi), &FormAsservi);
     Application->CreateForm(__classid(TFormSQLDB), &FormSQLDB);
     Application->CreateForm(__classid(TFormMagaz), &FormMagaz);
     Application->CreateForm(__classid(TFormBaie), &FormBaie);
     Application->CreateForm(__classid(TFormBox), &FormBox);
     Application->CreateForm(__classid(TFormListaCar), &FormListaCar);
     Application->CreateForm(__classid(TFormElBox), &FormElBox);
     Application->CreateForm(__classid(TFormAbbPlc), &FormAbbPlc);
     Application->CreateForm(__classid(TFormTipiBox), &FormTipiBox);
     Application->CreateForm(__classid(TFormSrvTcpAndroid), &FormSrvTcpAndroid);
     Application->CreateForm(__classid(TFormDisRiflettori), &FormDisRiflettori);
     Application->CreateForm(__classid(TFormStatoBaie), &FormStatoBaie);
     Application->CreateForm(__classid(TFormStatoMagaz), &FormStatoMagaz);
     Application->CreateForm(__classid(TFormCambioBatteria), &FormCambioBatteria);
     Application->CreateForm(__classid(TFormGrpDest), &FormGrpDest);
     Application->CreateForm(__classid(TFormGruppiBa), &FormGruppiBa);
     Application->CreateForm(__classid(TFormStatTgvEstesa), &FormStatTgvEstesa);
     Application->CreateForm(__classid(TFormDBGridGen), &FormDBGridGen);
     Application->CreateForm(__classid(TFormMacchine), &FormMacchine);
     Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  catch (...)
  {
     try
     {
        throw Exception("");
     }
     catch (Exception &exception)
     {
        Application->ShowException(&exception);
     }
  }
  return 0;
}
//---------------------------------------------------------------------------
