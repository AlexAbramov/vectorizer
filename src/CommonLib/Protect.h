#pragma once
#include "BitArray.h"

typedef enum ProtectStatus
{
	psWMIOk,
	psWMICreateLocatorError,
	psWMIBadNamespace,
	psWMICoSetProxiError,
	psWMIPropValError,
	psWMIPropTypeError,
	psWMIObjPropError,
	psWMIObjError,
	psWMIInstEnumError,
	psFileOk,
	psFileOpenError,
	psFileReadError,
	psFileWrongLength,
	psFileWrongChecksum,
	psProfileOk,
	psProfileOpenError,
	psProfileReadError,
	psProfileZeroLength,
	psProfileWrongLength,
	psProfileWrongChecksum,
	psLicFileOk,
	psLicFileOpenError,
	psLicFileReadError,
	psLicFileZeroLength,
	psLicFileWrongLength,
	psLicFileWrongChecksum,
	psKeyOk,
	psKeyWrong,
	psLicFileOverflow,
	psEmptyInitString,
	psInitialized,
	psTimeExpired,
	psDemo,
	psTrue,
	psFalse,
	psLast
};

class Protect
{
private:
// data
	bool Init();
	CWinApp* winApp;
	CString companyName;
	CString appName;
	CString copyright;
	CString version;
	Integer regNum;
	Integer ps;
	CMap<__int64,__int64,bool,bool> keys;
// methods
	CString RetrieveSerialNumber();
	CString LicFile();
	bool CheckFile();
	ProtectStatus ReadProfile(Integer& key);
	ProtectStatus ReadLicFile();
	bool IsKeyValid(const Integer& key);
	void StrongHash(const BYTE* buf,const int bufSize,Integer& hash);
	bool WriteProfile(const CString& key);
public:
	BYTE Radix()const;
	Protect();
	bool Init(CWinApp& winApp,LPCTSTR copyright,LPCTSTR version,const CTime* time=0);
	bool Register();
  CWinApp* GetApp(){return winApp;}
	LPCTSTR GetCompanyName(){return companyName;}
	LPCTSTR GetAppName(){return appName;}
	LPCTSTR GetCopyright(){return copyright;}
	LPCTSTR GetVersion(){return version;}
	CString GetRegNumber();
	CString GetStatus();
	CString GetModuleFileName();
	bool GetAt(int i){return ps.GetBit(i);}
	bool IsDemo(const CString& docId);
// AUX
	UINT Hash(const CString& s)const;
	void StrongHash(const CString& s,Integer& hash);
	void NormalizeKey(CString& s);
	bool IsKeyChecksumValid(const CString& key);
	CString GetDocId(const BYTE* buf,const int bufSize);
	WORD Checksum(const __int64& v)const;
	__int64 GetChecksumNumber();
	int HashCharSize();
	int IdCharSize();
	CString GetPath(void);
	bool IsDemo();
	bool IsRegistered();
};

extern Protect protect;
