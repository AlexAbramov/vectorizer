#include "stdafx.h"
#include "AboutDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AboutDlg::AboutDlg() : CDialog(IDD_ABOUTBOX)
{
	//{{AFX_DATA_INIT(AboutDlg)
	//}}AFX_DATA_INIT

	CWinApp* winApp=::AfxGetApp();
	ASSERT(winApp);
	curLink=winApp ? winApp->LoadCursor(IDC_LINK) : 0;
}

AboutDlg::~AboutDlg()
{
  if(curLink) DestroyCursor(curLink);
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AboutDlg)
	DDX_Control(pDX, IDC_URL, url);
	DDX_Control(pDX, IDC_EMAIL, email);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AboutDlg, CDialog)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL AboutDlg::OnInitDialog() 
{
	url.SubclassDlgItem(IDC_URL, this);
	email.SubclassDlgItem(IDC_EMAIL, this);
	logo.SubclassDlgItem(IDC_LOGO, this);
  url.GetWindowText(logo.m_link);

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_PRODNAME)->SetWindowText(::AfxGetApp()->m_pszAppName);
	GetDlgItem(IDC_COPYRIGHT)->SetWindowText("(C) 2008 Alexander Abramov");
	GetDlgItem(IDC_VERSION)->SetWindowText("Version 1.0.3");

	CString s;
	s.LoadString(IDS_SITE);
	url.SetWindowText(s);
	s = "";
	s.LoadString(IDS_SUPPORTEMAIL);
	email.SetWindowText(s);

	CString caption=_T("About Geomethod Vectorizer");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL AboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
  if (pWnd->GetSafeHwnd() ==  url.GetSafeHwnd()||
		  pWnd->GetSafeHwnd() ==  email.GetSafeHwnd())
  {
    SetCursor(curLink);
    return true;
  }

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
