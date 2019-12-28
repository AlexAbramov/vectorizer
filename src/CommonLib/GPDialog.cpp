// GPDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CommonLib.h"
#include "GPDialog.h"
#include "Protect.h"

// GPDialog dialog

IMPLEMENT_DYNAMIC(GPDialog, CDialog)
GPDialog::GPDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GPDIALOG, pParent)
{
	CWinApp* winApp=::AfxGetApp();
	ASSERT(winApp);
	curLink=winApp ? winApp->LoadCursor(IDC_LINK) : 0;
}

GPDialog::~GPDialog()
{
  if(curLink) DestroyCursor(curLink);
}

void GPDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URL, url);
}


BEGIN_MESSAGE_MAP(GPDialog, CDialog)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL GPDialog::OnInitDialog() 
{
	url.SubclassDlgItem(IDC_URL, this);

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

//	CString s;
//	s.LoadString(IDS_SITE);
//	url.SetWindowText(s);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL GPDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
  if (pWnd->GetSafeHwnd() ==  url.GetSafeHwnd())
  {
    SetCursor(curLink);
    return true;
  }

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
