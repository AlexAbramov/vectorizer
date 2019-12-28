// VectorizerDoc.cpp : implementation of the CVectorizerDoc class
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "VectorizerDoc.h"
#include "OptionsDialog.h"
#include "DocManager.h"
#include "Param.h"

/////////////////////////////////////////////////////////////////////////////
// CVectorizerDoc

IMPLEMENT_DYNCREATE(CVectorizerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVectorizerDoc, CDocument)
	ON_COMMAND(ID_VIEW_RASTER, OnViewRaster)
	ON_COMMAND(ID_VIEW_VECTOR, OnViewVector)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RASTER, OnUpdateViewRaster)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VECTOR, OnUpdateViewVector)
	ON_COMMAND(ID_VECTORIZE, OnVectorize)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_VECTORIZE, OnUpdateVectorize)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CVectorizerDoc, CDocument)
	DISP_FUNCTION_ID(CVectorizerDoc, "Open", dispidOpen, Open, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CVectorizerDoc, "Save", dispidSave, Save, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CVectorizerDoc, "Vectorize", dispidVectorize, Vectorize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CVectorizerDoc, "SetOptions", dispidSetOptions, SetOptions1, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CVectorizerDoc, "GetOptions", dispidGetOptions, GetOptions1, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CVectorizerDoc, "GetVersion", dispidGetVersion, GetVersion, VT_BSTR, VTS_NONE)
END_DISPATCH_MAP()

// Note: we add support for IID_IVectorizer to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {EB9B775A-285D-483E-9285-E41EF4C5E257}
static const IID IID_IVectorizer =
{ 0xeb9b775a, 0x285d, 0x483e, { 0x92, 0x85, 0xe4, 0x1e, 0xf4, 0xc5, 0xe2, 0x57 } };

BEGIN_INTERFACE_MAP(CVectorizerDoc, CDocument)
	INTERFACE_PART(CVectorizerDoc, IID_IVectorizer, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorizerDoc construction/destruction

CVectorizerDoc::CVectorizerDoc(): image(0), demo(true),bTrue(true)
, viewRaster(true)
, viewVector(true)
{
	// TODO: add one-time construction code here

	EnableAutomation();

	AfxOleLockApp();
}

CVectorizerDoc::~CVectorizerDoc()
{
	AfxOleUnlockApp();
	if(image) delete image;
}

BOOL CVectorizerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerDoc serialization

void CVectorizerDoc::Serialize(CArchive& ar)
{
//	CString path=ar.GetFile()->GetFilePath();
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		if(vectorizer.IsDemo())
		{
			CString s;
			s.LoadString(IDS_RESTRICTION);
			Message(s);
		}
		if(outExt=="dxf") vectorizer.batch.SaveDxf(ar);
		else if(outExt=="vml") vectorizer.batch.SaveVml(ar);
		else if(outExt=="svg") vectorizer.batch.SaveSvg(ar);
		else if(outExt=="htm") vectorizer.batch.SaveHtml(ar);
		else Message("Non supported format: "+outExt);
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerDoc diagnostics

#ifdef _DEBUG
void CVectorizerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVectorizerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVectorizerDoc commands



void CVectorizerDoc::OnViewRaster()
{
	// TODO: Add your command handler code here
	viewRaster=!viewRaster;
	if(!viewVector&&!viewRaster) viewVector=!viewVector;
	UpdateAllViews(0);
}

void CVectorizerDoc::OnViewVector()
{
	// TODO: Add your command handler code here
	viewVector=!viewVector;
	if(!viewVector&&!viewRaster) viewRaster=!viewRaster;
	UpdateAllViews(0);
}

void CVectorizerDoc::OnUpdateViewRaster(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(viewRaster);
}

void CVectorizerDoc::OnUpdateViewVector(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(viewVector);
}

void CVectorizerDoc::OnVectorize()
{
	// TODO: Add your command handler code here
	if(!image||image->IsNull()) return;

	HDC hdc=image->GetDC();
	vectorizer.batch.demo=bTrue&&demo;
	int res=vectorizer.Vectorize(hdc);
	image->ReleaseDC();
	
	UpdateAllViews(0);
}

void CVectorizerDoc::OnOptions()
{
	// TODO: Add your command handler code here
	OptionsDialog dlg(_T("Options"));
	dlg.DoModal();
}

VARIANT_BOOL CVectorizerDoc::Open(LPCTSTR pathName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your dispatch handler code here
	return OnOpenDocument(pathName);
//	return VARIANT_TRUE;
}

VARIANT_BOOL CVectorizerDoc::Save(LPCTSTR pathName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your dispatch handler code here

	return OnSaveDocument(pathName);
}

VARIANT_BOOL CVectorizerDoc::Vectorize(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your dispatch handler code here
	OnVectorize();
	return VARIANT_TRUE;
}

VARIANT_BOOL CVectorizerDoc::SetOptions1(LPCTSTR options)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your dispatch handler code here
  return SetOptions(options);
}

bool CVectorizerDoc::AddOption(CString& key, CString& value)
{
	//int ival=_ttoi(value);
	if(key==_T("maxd"))
	{
//	  _tstof(
    return true;  
	}
	Message(_T("Wrong parameter: ")+key);
	return false;
}

bool CVectorizerDoc::SetOptions(LPCTSTR options)
{
	int length=_tcslen(options);
	int	i1=-1;
	int i2=-1;
	bool bError=false;
	for(int i=0;i<length&&!bError;i++)
	{
		TCHAR c=options[i];
		switch(c)
		{
		case _T('/'):
		case _T('-'):
			if(i1>=0) bError=true;
			else i1=i;
			break;
		case _T('='):
		case _T(':'):
			if(i2>=0) bError=true;
			else i2=i;
			break;
		case _T(' '):
		case _T(','):
		case _T(';'):
		case 0:
			if(i1<0||i2<0||i2-i1<2||i-i2<2) bError=true;
			else
			{
        CString param(options+i1+1,i2-i1-1);
				CString value(options+i2+1,i-i2-1);
			  if(!AddOption(param,value)) return false;
				i1=i2=0;
			}
		}
	}
	if(bError)
	{
		CString s=_T("Wrong format of options string: ");
		Message(s+options);
    return false;
	}
	else return true;
}

CString CVectorizerDoc::GetOptions()
{
	CString res;
	return res;
}

BSTR CVectorizerDoc::GetOptions1(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your dispatch handler code here

	return GetOptions().AllocSysString();
}

bool CVectorizerDoc::GetSize(SIZE& size)
{
	if(IsValidImage(image))
	{
		size.cx=image->GetWidth();
    size.cy=image->GetHeight();
		return true;
	}
	size.cx=1;
	size.cy=1;
	return true;
}

BOOL CVectorizerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	vectorizer.batch.demo=bTrue&&demo;
	outExt=GetExt(lpszPathName);
	outExt.MakeLower();
	if(theApp.IsRasterExt(outExt))
	{
	  return SUCCEEDED(image->Save(lpszPathName));
	}
	if(theApp.IsVectorExt(outExt))
	{
	  return CDocument::OnSaveDocument(lpszPathName);
	}
	Message("Non supported graphics format");
	return FALSE;
}

BOOL CVectorizerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	demo=theApp.IsDemo();
	vectorizer.batch.Clear();
	ext=GetExt(lpszPathName);
	ext.MakeLower();
	DocManager* docManager=theApp.GetDocManager();
	if(docManager&&docManager->image)
	{
		if(image) delete image;
		image=docManager->image;
		docManager->image=0;
	}
	else
	{
	  if(theApp.IsRasterExt(ext))
		{
		  if(!image) image=new CImage();
			else if(!image->IsNull()) image->Destroy();
			image->Load(lpszPathName);
		}
	}
	if(IsValidImage(image))
	{
		CWnd* wnd=theApp.GetMainWnd();
		CString s=theApp.m_pszAppName;
		s+=_T(" - ");
		s+=GetFileName(lpszPathName);
		if(wnd) wnd->SetWindowText(s);
		return TRUE;
	}

//	if (!CDocument::OnOpenDocument(lpszPathName))	return FALSE;

	// TODO:  Add your specialized creation code here

	return FALSE;
}

void CVectorizerDoc::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!vectorizer.IsEmpty());
}

void CVectorizerDoc::OnUpdateVectorize(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(IsValidImage(image));
}

BSTR CVectorizerDoc::GetVersion(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CString strResult;
	strResult.LoadString(IDS_VERSION);

	// TODO: Add your dispatch handler code here

	return strResult.AllocSysString();
}
