// RegistrationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RegistrationDialog.h"

/////////////////////////////////////////////////////////////////////////////
// RegistrationDialog dialog

RegistrationDialog::RegistrationDialog(CWnd* pParent/*=NULL*/)
	: CDialog(IDD_REGISTRATION, pParent)
{
	//{{AFX_DATA_INIT(RegistrationDialog)
	key = _T("");
	regNum = _T("");
	//}}AFX_DATA_INIT
	regNum=protect.GetRegNumber();
	CWinApp* winApp=protect.GetApp();
	ASSERT(winApp);
	curLink=winApp ? winApp->LoadCursor(IDC_LINK) : 0;
}

void RegistrationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RegistrationDialog)
	DDX_Control(pDX, IDC_REGSITE, regSite);
	DDX_Control(pDX, IDOK, btnOk);
	DDX_Text(pDX, IDC_KEY, key);
	DDX_Text(pDX, IDC_REGNUM, regNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RegistrationDialog, CDialog)
	//{{AFX_MSG_MAP(RegistrationDialog)
	ON_EN_CHANGE(IDC_KEY, OnChangeKey)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RegistrationDialog message handlers

void RegistrationDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	ASSERT(protect.IsKeyChecksumValid(key));
	CDialog::OnOK();
}

BOOL RegistrationDialog::OnInitDialog() 
{
	regSite.SubclassDlgItem(IDC_REGSITE, this);

	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
//	edit1.LimitText(10);
//	edit1.SetWindowText(number);
//	edit2.LimitText(5);
	btnOk.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RegistrationDialog::OnChangeKey() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	btnOk.EnableWindow(protect.IsKeyChecksumValid(key));
}

BOOL RegistrationDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
  if (pWnd->GetSafeHwnd() ==  regSite.GetSafeHwnd())
  {
    SetCursor(curLink);
    return true;
  }
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
