// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'IcsLogger.pas' rev: 6.00

#ifndef IcsLoggerHPP
#define IcsLoggerHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Classes.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Icslogger
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS ELoggerException;
class PASCALIMPLEMENTATION ELoggerException : public Sysutils::Exception 
{
	typedef Sysutils::Exception inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall ELoggerException(const AnsiString Msg) : Sysutils::Exception(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall ELoggerException(const AnsiString Msg, const System::TVarRec * Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall ELoggerException(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall ELoggerException(int Ident, const System::TVarRec * Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall ELoggerException(const AnsiString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall ELoggerException(const AnsiString Msg, const System::TVarRec * Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall ELoggerException(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall ELoggerException(System::PResStringRec ResStringRec, const System::TVarRec * Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~ELoggerException(void) { }
	#pragma option pop
	
};


#pragma option push -b-
enum TLogOption { loDestEvent, loDestFile, loDestOutDebug, loAddStamp, loWsockErr, loWsockInfo, loWsockDump, loSslErr, loSslInfo, loSslDump, loProtSpecErr, loProtSpecInfo, loProtSpecDump };
#pragma option pop

typedef Set<TLogOption, loDestEvent, loProtSpecDump>  TLogOptions;

#pragma option push -b-
enum TLogFileOption { lfoAppend, lfoOverwrite };
#pragma option pop

#pragma option push -b-
enum TNTEventType { etError, etWarning, etInformation, etAuditSuccess, etAuditFailure };
#pragma option pop

typedef void __fastcall (__closure *TIcsLogEvent)(System::TObject* Sender, TLogOption LogOption, const AnsiString Msg);

class DELPHICLASS TIcsLogger;
class PASCALIMPLEMENTATION TIcsLogger : public Classes::TComponent 
{
	typedef Classes::TComponent inherited;
	
protected:
	TLogOptions FLogOptions;
	TIcsLogEvent FOnIcsLogEvent;
	AnsiString FLogFileName;
	Classes::TFileStream* FLogFile;
	TLogFileOption FLogFileOption;
	_RTL_CRITICAL_SECTION FLock;
	void __fastcall Lock(void);
	void __fastcall UnLock(void);
	void __fastcall WriteToLogFile(const AnsiString S);
	void __fastcall SetLogFileOption(const TLogFileOption Value);
	void __fastcall SetLogOptions(const TLogOptions Value);
	void __fastcall SetLogFileName(const AnsiString Value);
	void __fastcall SetOnIcsLogEvent(const TIcsLogEvent Value);
	void __fastcall InternalOpenLogFile(void);
	void __fastcall InternalCloseLogFile(void);
	
public:
	__fastcall virtual TIcsLogger(Classes::TComponent* AOwner);
	__fastcall virtual ~TIcsLogger(void);
	void __fastcall OpenLogFile(void);
	void __fastcall CloseLogFile(void);
	void __fastcall DoDebugLog(System::TObject* Sender, TLogOption LogOption, const AnsiString Msg);
	
__published:
	__property TLogFileOption LogFileOption = {read=FLogFileOption, write=SetLogFileOption, nodefault};
	__property AnsiString LogFileName = {read=FLogFileName, write=SetLogFileName};
	__property TLogOptions LogOptions = {read=FLogOptions, write=SetLogOptions, nodefault};
	__property TIcsLogEvent OnIcsLogEvent = {read=FOnIcsLogEvent, write=SetOnIcsLogEvent};
};


//-- var, const, procedure ---------------------------------------------------
static const Shortint TIcsLoggerVersion = 0x65;
extern PACKAGE AnsiString CopyRight;
#define LogAllOptErr (System::Set<TLogOption, loDestEvent, loProtSpecDump> () << TLogOption(4) << TLogOption(7) << TLogOption(10) )
#define LogAllOptInfo (System::Set<TLogOption, loDestEvent, loProtSpecDump> () << TLogOption(4) << TLogOption(5) << TLogOption(7) << TLogOption(8) << TLogOption(10) << TLogOption(11) )
#define LogAllOptDump (System::Set<TLogOption, loDestEvent, loProtSpecDump> () << TLogOption(4) << TLogOption(5) << TLogOption(6) << TLogOption(7) << TLogOption(8) << TLogOption(9) << TLogOption(10) << TLogOption(11) << TLogOption(12) )
extern PACKAGE void __fastcall Register(void);
extern PACKAGE AnsiString __fastcall IcsLoggerAddTimeStamp();

}	/* namespace Icslogger */
using namespace Icslogger;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// IcsLogger
