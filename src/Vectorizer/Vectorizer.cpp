// Vectorizer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "DocManager.h"

#include "MainFrm.h"
#include "VectorizerDoc.h"
#include "VectorizerView.h"
#include "..\CommonLib\AboutDlg.h"
#include "..\CommonLib\GPDialog.h"

class InitGDIPlus
{
	private:
		ULONG_PTR m_dwToken;
	public:
		InitGDIPlus():m_dwToken(0){}
		~InitGDIPlus(){if(m_dwToken) Gdiplus::GdiplusShutdown( m_dwToken );}
		bool Init()
		{
			if(m_dwToken) return true;
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartupOutput output;
			Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, &output );
			return status == Gdiplus::Ok;
		}
};

static InitGDIPlus initGDIPlus;

static bool InitGDIPlus()
{
	bool gdiPlusInitialized=false;
	try
	{
	  gdiPlusInitialized=initGDIPlus.Init();
	}
	catch(...)
	{
	}
	if(gdiPlusInitialized) return true;
	GPDialog dlg;
	dlg.DoModal();
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerApp

BEGIN_MESSAGE_MAP(CVectorizerApp, CWinApp)
	//{{AFX_MSG_MAP(CVectorizerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorizerApp construction

CVectorizerApp::CVectorizerApp(): CWinApp(_T("Geomethod Vectorizer")),runAutomated(false),demo(true)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	EnableHtmlHelp();
	const LPCTSTR rext[]={"bmp","dib","rle","gif","jpg","jpeg","tif","tiff","png","ico","wmf","emf",0};
	const LPCTSTR vext[]={"dxf","vml","svg","htm",0};

	for(int i=0;rext[i];i++) rasterExt.AddTail(rext[i]);
	for(int i=0;vext[i];i++) vectorExt.AddTail(vext[i]);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVectorizerApp object

CVectorizerApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {F80991C4-3072-4D55-B40B-FC9DAB59D9BD}
static const CLSID clsid =
{ 0xf80991c4, 0x3072, 0x4d55, { 0xb4, 0xb, 0xfc, 0x9d, 0xab, 0x59, 0xd9, 0xbd } };

const GUID CDECL BASED_CODE _tlid =
		{ 0x99D81B0E, 0x4A29, 0x4AD1, { 0x9F, 0x1A, 0x4F, 0x26, 0x65, 0x03, 0x77, 0x70 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

/////////////////////////////////////////////////////////////////////////////
// CVectorizerApp initialization

class VectorizerDocTemplate: public CSingleDocTemplate
{
public:
	VectorizerDocTemplate(UINT nIDResource,CRuntimeClass* pDocClass,CRuntimeClass* pFrameClass,CRuntimeClass* pViewClass):
	  CSingleDocTemplate(nIDResource,pDocClass,pFrameClass,pViewClass)
	{
	}
/*	virtual Confidence MatchDocType( LPCTSTR lpszPathName, CDocument*& rpDocMatch )
	{
		Confidence match=CSingleDocTemplate::MatchDocType(lpszPathName,rpDocMatch);
		if(match==yesAlreadyOpen&&rpDocMatch&&rpDocMatch->IsModified())
		{
			match=yesAttemptNative;
			rpDocMatch=0;
		}
		return match;
	}*/
	virtual CDocument* OpenDocumentFile( LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE )
	{
		return CSingleDocTemplate::OpenDocumentFile(lpszPathName,bMakeVisible);
	}
};

BOOL CVectorizerApp::InitInstance()
{
	CWinApp::InitInstance();

	if(!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	if(!InitGDIPlus()) return FALSE;

	SetRegistryKey(_T("Geomethod Software"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

  ASSERT(m_pDocManager == NULL);
  m_pDocManager = new DocManager;

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new VectorizerDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVectorizerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CVectorizerView));
	AddDocTemplate(pDocTemplate);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		// Note: SDI applications register server objects only if /Embedding
		//   or /Automation is present on the command line.

	// Enable DDE Execute open
//	EnableShellOpen();
//	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CString copyright,version;
	copyright.LoadString(IDS_COPYRIGHT);
	version.LoadString(IDS_VERSION);

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		runAutomated=true;
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

	}
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
//		UnregisterShellFileTypes();
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT, NULL, NULL, FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}
	else
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
		if(__argc>2)
		{
			DoBatch();
			return false;
		}
	}

		// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	POSITION pos=pDocTemplate->GetFirstDocPosition();
	while(pos)
	{
		CDocument* doc=pDocTemplate->GetNextDoc(pos);
		if(doc&&doc->IsKindOf(RUNTIME_CLASS(CVectorizerDoc)))
		{
			((CVectorizerDoc*)doc)->demo=demo;
		}
	}

	if(RunAutomated()) this->HideApplication();
	else
	{
		m_pMainWnd->ShowWindow(SW_SHOW);
	  m_pMainWnd->UpdateWindow();
		m_pMainWnd->DragAcceptFiles();
	}

	return TRUE;
}

// App command to run the dialog
void CVectorizerApp::OnAppAbout()
{
	AboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerApp message handlers

void CVectorizerApp::DoBatch(void)
{
	CString fin=__targv[1];
	
	CVectorizerDoc doc;
	if(!doc.OnOpenDocument(fin)) return;
	CString fout=__targv[2];
	for (int i = 3; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];
		doc.SetOptions(pszParam);
	}
	if(doc.Vectorize())
	{
		doc.OnSaveDocument(fout);
	}
}

int CVectorizerApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}

bool CVectorizerApp::HasExt(LPCTSTR ext,CStringList& slist)
{
	POSITION pos=slist.GetHeadPosition();
	while(pos)
	{
		const CString& s=slist.GetNext(pos);
		if(s.CompareNoCase(ext)==0)
			return true;
	}
	return false;
}

bool CVectorizerApp::IsDemo()const
{
	return demo;
}