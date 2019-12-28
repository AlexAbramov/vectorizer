// Protect.cpp: implementation of the Protect class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CommonLib.h"
#include "BitArray.h"
#include "Protect.h"
#include "RegistrationDialog.h"
#include "ErrorReport.h"
#include <objbase.h>
#include <comdef.h>
#include <wbemcli.h>
#include <math.h>
#define __MINMAX_DEFINED
#include <SHA.h>

static const UINT hashByteSize=10;
static const BYTE radix=62;

static LPCTSTR regSectionName=_T("Settings");
static LPCTSTR regValueName1=_T("Reg1");
static LPCTSTR regValueName2=_T("Reg2");

static const UINT hashBitSize=hashByteSize<<3;
static void NormalizeString(CString& s,const int size){while(s.GetLength()<size) s=_T('0')+s;}
static int CharSize(const UINT byteSize)
{
	Integer bigInt;
	bigInt.BitSize(byteSize<<3);
	for(int i=0;i<byteSize;i++) bigInt.SetByte(i,0xFF);
	CString s=bigInt.ToNRadixString(radix);
	return s.GetLength();
}

Protect protect;

Protect::Protect()
{
	ps.BitSize(64);
	ps.Clear();
	winApp=0;
}

bool Protect::Init(CWinApp& winApp,LPCTSTR copyright,LPCTSTR version,const CTime* time)
{
	this->winApp=&winApp;
	this->copyright=copyright;
	this->version=version;
	companyName=winApp.m_pszRegistryKey;
	appName=winApp.m_pszAppName;
	ps.SetBit(psDemo);
	ps.SetBit(psTrue);
	ps.SetBit(psFalse,false);

	if(companyName.IsEmpty()||appName.IsEmpty()||this->copyright.IsEmpty()||this->version.IsEmpty())
	{
		ps.SetBit(psEmptyInitString);
		goto initFailed;
	}
	this->copyright+=_T(' ')+companyName;
#ifdef _DEBUG
  this->version+=_T(" Debug");
#endif
	free((void*)winApp.m_pszHelpFilePath);
  winApp.m_pszHelpFilePath=_tcsdup(GetPath()+winApp.m_pszExeName+_T(".chm"));

	srand(CTime::GetCurrentTime().GetTime());
  if(time)
	{
		CTime curTime=CTime::GetCurrentTime();
		if(curTime>(*time))
		{
			CTimeSpan timeSpan(curTime-(*time));
			int nhours=timeSpan.GetTotalHours();
			double p=exp(-.01*nhours);
			for(int i=0;i<9;i++) rand();
			if(rand()>p*RAND_MAX)
			{
  		  ps.SetBit(psTimeExpired);
				goto initFailed;
			}
		}
	}
	if(Init()) return true;
/*	if(protect.GetAt(psKeyWrong))
	{
	  Message(_T("Your licence is valid for operation system with another serial number. See help for details."));
    return false;
	}*/
initFailed:
	ReportError(GetStatus());
	return false;
}

CString Protect::GetStatus()
{
	return ps.ToNRadixString(radix);
}

static __int64 checksum=0xE48F07A50C221B93;

__int64 Protect::GetChecksumNumber()
{
	return checksum;
}

bool Protect::CheckFile()
{
	CFile file;
	CFileException ex;
	if(!file.Open(GetModuleFileName(),CFile::modeRead|CFile::shareDenyWrite,&ex))
	{
#ifdef _DEBUG
		ex.ReportError();
#endif
		ps.SetBit(psFileOpenError);
		return 0;
	}

	UINT sum=0xFF00;
	const int bufSize=1<<12;
	BYTE buf[bufSize];
	int n=0;
	while((n=file.Read(&buf,bufSize))>0)
	{
		for(int i=0;i<n;i++) sum+=buf[i];
	}
	if(n<0)
	{
		ps.SetBit(psFileReadError);
		return false;
	}
#ifdef _DEBUG
	ps.SetBit(psFileOk);
	return true;
#endif
	BYTE* ptr=(BYTE*)&checksum;
	for(int i=0;i<8;i++) sum-=ptr[i];
	UINT count=file.GetLength()+0xFF00;
	file.Close();
	if(count!=((DWORD*)&checksum)[0])
	{
		ps.SetBit(psFileWrongLength);
		return false;
	}
	if(sum==((DWORD*)&checksum)[1])
	{
		ps.SetBit(psFileOk);
		return true;
	}
	else
	{
		ps.SetBit(psFileWrongChecksum);
		return false;
	}
}

CString Protect::RetrieveSerialNumber()
{
	CString resStr;
	IWbemLocator *pIWbemLocator =0;

	if(CoCreateInstance(CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID *) &pIWbemLocator) != S_OK)
	{
	  ps.SetBit(psWMICreateLocatorError);
		return resStr;
	}

	IWbemServices *m_pIWbemServices=0;
	
	CString snamespace=_T("\\\\.\\root\\cimv2");
	if(pIWbemLocator->ConnectServer(snamespace.AllocSysString(),
							NULL,   //using current account for simplicity
							NULL,	//using current password for simplicity
							0L,		// locale
							0L,		// securityFlags
							NULL,	// authority (domain for NTLM)
							NULL,	// context
							&m_pIWbemServices) != S_OK) 
	{
	  ps.SetBit(psWMIBadNamespace);
  	pIWbemLocator->Release();
		return resStr;
	}

		// Switch the security level to IMPERSONATE so that provider(s)
	// will grant access to system-level objects, and so that
	// CALL authorization will be used.
	HRESULT  hRes=CoSetProxyBlanket(m_pIWbemServices,	// proxy
		RPC_C_AUTHN_WINNT,				// authentication service
		RPC_C_AUTHZ_NONE, 				// authorization service
		NULL,							// server principle name
		RPC_C_AUTHN_LEVEL_CALL,			// authentication level
		RPC_C_IMP_LEVEL_IMPERSONATE,	// impersonation level
		NULL,							// identity of the client
		EOAC_NONE);						// capability flags
	if(!SUCCEEDED(hRes))
	{
	  ps.SetBit(psWMICoSetProxiError);
  	pIWbemLocator->Release();
		return resStr;
	}

	CString className = _T("Win32_OperatingSystem");
	IEnumWbemClassObject *pEnum = NULL;

	hRes = m_pIWbemServices->CreateInstanceEnum(
		className.AllocSysString(),			// name of class
		0,
		NULL,
		&pEnum);	// pointer to enumerator

	if(SUCCEEDED(hRes))
	{
		ULONG uReturned = 1;
		IWbemClassObject *pStorage = NULL;
		hRes = pEnum->Next(
				2000,			// timeout in ml seconds
				1,				// return just one storage device
				&pStorage,	// pointer to storage device
				&uReturned);	// number obtained: one or zero

		if(SUCCEEDED(hRes))
		{
			VARIANT pVal;
			VariantClear(&pVal);

			CString propName(_T("SerialNumber"));
			hRes = pStorage->Get(propName.AllocSysString(),0L,&pVal,NULL,NULL);
			if (SUCCEEDED(hRes))
			{
				if(pVal.vt==VT_BSTR)
				{
					resStr=pVal.bstrVal;
					if(resStr.GetLength()==0)	ps.SetBit(psWMIPropValError);
					else
					{
						ps.SetBit(psWMIOk);
					}
				}
				else ps.SetBit(psWMIPropTypeError);
			}
   	  else ps.SetBit(psWMIObjPropError);
			if(pStorage) pStorage->Release();
		}
		else ps.SetBit(psWMIObjError);
		if (pEnum)  pEnum->Release();
  } 
	else ps.SetBit(psWMIInstEnumError);

	pIWbemLocator->Release();
	return resStr;
}

ProtectStatus Protect::ReadProfile(Integer& key)
{
	BYTE* buf=0;//[hashByteSize+1];
	UINT nbytes=hashByteSize+1;
	if(!winApp->GetProfileBinary(regSectionName,regValueName1,&buf,&nbytes)) return psProfileOpenError;
	key.BitSize(hashBitSize);
	memcpy(key.ByteArray(),buf,hashByteSize);
	if(buf){delete []buf;buf=0;}
	if(nbytes==0) return psProfileZeroLength;
	if(nbytes!=hashByteSize) return psProfileWrongLength;
	if(key.Checksum()!=~Hash(appName)) return psProfileWrongChecksum;
	return psProfileOk;
}

ProtectStatus Protect::ReadLicFile()
{
	CFile licFile;
	if(!licFile.Open(LicFile(),CFile::modeRead))
	  return psLicFileOpenError;
	ProtectStatus res=psLicFileOk;
	const WORD chkSum=~Hash(appName);
	__int64 key;
	for(int count=0;;count++)
	{
		if(count>1024){res=psLicFileOverflow;break;}
	  UINT n=licFile.Read(&key,8);
	  if(n==8)
		{
			if(Checksum(key)==chkSum)
			{
			  keys.SetAt(key,0);
				continue;
			}
			res=psLicFileWrongChecksum;
			break;
		}
		if(n!=0) res=psLicFileWrongLength;
		break;
	}
	if(res!=psLicFileOk) keys.RemoveAll();
	return res;
}

bool Protect::WriteProfile(const CString& key)
{
	Integer ikey;
	ikey.BitSize(hashBitSize);
	if(!ikey.FromNRadixString(key,radix)||ikey.Checksum()!=~Hash(appName))
	{
		Message(_T("Wrong key"));
		return false;
	}
	if(!winApp->WriteProfileBinary(regSectionName,regValueName1,ikey.ByteArray(),hashByteSize))
	{
		Message(_T("Profile write error"));
		return false;
	}
	BYTE* ar=ikey.ByteArray();
	for(int i=0;i<ikey.ByteSize();i++)
	{
		ar[i]+=(BYTE) rand();
	}
	if(!winApp->WriteProfileBinary(regSectionName,regValueName2,ikey.ByteArray(),hashByteSize))
	{
		Message(_T("Profile write error"));
		return false;
	}
	return true;
}

void Protect::StrongHash(const CString& s,Integer& hash)
{
	const int inputSize=s.GetLength();
	BYTE* inputData=new BYTE[inputSize];
	for(int i=0;i<inputSize;i++) inputData[i]=s[i];
	StrongHash(inputData,inputSize,hash);
	delete []inputData;
}

void Protect::StrongHash(const BYTE* inputData,const int inputSize,Integer& hash)
{
	const int outputSize=20;
	BYTE outputData[20];
	for(int i=0;i<outputSize;i++) outputData[i]=0;
	SHA sha;
	sha.Update(inputData,inputSize);
	sha.Final(outputData);
	ASSERT(hashByteSize<=outputSize);
	hash.BitSize(hashBitSize);
	hash.Clear();
	for(i=0;i<hashByteSize;i++) hash.SetByte(i,outputData[i]);
}

bool Protect::IsKeyChecksumValid(const CString& key)
{
	if(key.GetLength()!=HashCharSize()) return false;
	Integer ikey;
	if(!ikey.FromNRadixString(key,radix)) return false;
	return ikey.Checksum()==~Hash(appName);
}

UINT Protect::Hash(const CString &s)const
{
	UINT sum=0;
	for(int i=0;i<s.GetLength();i++)
	{
		UINT k=s[i];
		k<<=(i%4)<<3;
		sum+=k;		
	}
	return sum;
}

bool Protect::IsKeyValid(const Integer& key)
{
	Integer keyOrig;
	StrongHash(GetRegNumber()+appName,keyOrig);
	keyOrig.Checksum(~Hash(appName));
	ASSERT(keyOrig.Checksum()==~Hash(appName));
	if(keyOrig==key)
	{
		ps.SetBit(psKeyOk);
	  ps.SetBit(psDemo,false);
		return true;
	}
	ps.SetBit(psKeyWrong);
	return false;
}

CString Protect::LicFile()
{
	CString licFile=GetModuleFileName();
//	ASSERT(licFile.Right(3).CompareNoCase(_T("exe"))==0);
	int length=licFile.GetLength();
	licFile.SetAt(length-3,_T('l'));
	licFile.SetAt(length-2,_T('i'));
	licFile.SetAt(length-1,_T('c'));
	return licFile;
}

CString Protect::GetModuleFileName()
{
	TCHAR szBuff[_MAX_PATH];
	szBuff[0]=0;
	VERIFY(::GetModuleFileName(winApp->m_hInstance, szBuff, _MAX_PATH));
	return CString(szBuff);
}

CString Protect::GetRegNumber()
{
	CString s=regNum.ToNRadixString(radix);
	ASSERT(s.GetLength()<=HashCharSize());
	NormalizeString(s,HashCharSize());
	return s;
}

CString Protect::GetDocId(const BYTE *buf, const int bufSize)
{
	Integer id;
	StrongHash(buf,bufSize,id);
	ASSERT(id.ByteSize()==hashByteSize);
	for(int i=8;i<id.ByteSize();i++) id.SetByte(i,0);
	id.WordChecksum((WORD)Hash(appName));
	CString s=id.ToNRadixString(radix);
	ASSERT(s.GetLength()<=IdCharSize());
	NormalizeString(s,IdCharSize());
	return s;
}

bool Protect::IsDemo(const CString& docId)
{
	if(GetAt(psKeyOk)&&(GetAt(psProfileOk)||GetAt(psLicFileOk))) return false;
	__int64 key=0;
	Integer ikey;
	StrongHash(docId+appName,ikey);
	ASSERT(ikey.ByteSize()==hashByteSize);
	for(int i=8;i<ikey.ByteSize();i++) ikey.SetByte(i,0);
	ikey.WordChecksum((WORD)~Hash(appName));
	ASSERT(ikey.WordChecksum()==(WORD)~Hash(appName));
	memcpy(&key,ikey.ByteArray(),8);
	bool b;
	return !keys.Lookup(key,b);
}

void Protect::NormalizeKey(CString &s)
{
	NormalizeString(s,HashCharSize());
}

WORD Protect::Checksum(const __int64 &v) const
{
	WORD sum=0;
	for(int i=0;i<4;i++) sum+=((WORD*)&v)[i];
	return sum;
}

bool Protect::Register()
{
	RegistrationDialog dlg;
	if(dlg.DoModal()==IDOK)
	{
		protect.WriteProfile(dlg.GetKey());
		CString s;
		s.LoadString(IDS_RESTARTPROGRAM);
		Message(s);
		return true;
	}
	return false;
}

bool Protect::Init()
{
	CString serNum=RetrieveSerialNumber();
	if(serNum.GetLength()==0) return false;
	StrongHash(serNum+appName,regNum);
	regNum.Checksum(Hash(appName));
	ASSERT(regNum.Checksum()==Hash(appName));
	if(!CheckFile()) return false;
	Integer ikey;
	ProtectStatus psReadProfile=ReadProfile(ikey);
	ps.SetBit(psReadProfile);
	if(psReadProfile!=psProfileOk)
	{
		if(psReadProfile!=psProfileOpenError) return false;
		ProtectStatus psReadLic=ReadLicFile();
		ps.SetBit(psReadLic);
		ps.SetBit(psInitialized);
		return true;
	}
	IsKeyValid(ikey);
	ps.SetBit(psInitialized);
	return true;
}

int Protect::HashCharSize()
{
	return CharSize(hashByteSize);
}

int Protect::IdCharSize()
{
  return CharSize(8);
}

BYTE Protect::Radix() const
{
  return radix;
}

CString Protect::GetPath(void)
{
	CString s=GetModuleFileName();
	int i=s.ReverseFind(_T('\\'));
	return i<0 ? _T("") : s.Left(i+1);
}

bool Protect::IsDemo()
{
	if(!GetAt(psInitialized)) return true;
	return GetAt(psTrue)&&GetAt(psDemo);
}

bool Protect::IsRegistered()
{
	if(!GetAt(psInitialized)) return false;
	return GetAt(psFalse)||protect.GetAt(psKeyOk);
}