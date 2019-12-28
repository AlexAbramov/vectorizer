#include "stdafx.h"
#include "ErrorReport.h"

void ReportError(LPCTSTR report)
{
  ErrorReport dlg(report);
  dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// ErrorReport dialog


ErrorReport::ErrorReport(LPCTSTR report,CWnd* pParent /*=NULL*/)
	: CDialog(ErrorReport::IDD, pParent)
{
	//{{AFX_DATA_INIT(ErrorReport)
	//}}AFX_DATA_INIT
	this->report=report;
}


void ErrorReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ErrorReport)
	DDX_Text(pDX, IDC_ER_EDIT1, report);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ErrorReport, CDialog)
	//{{AFX_MSG_MAP(ErrorReport)
	ON_BN_CLICKED(IDC_SEND_REPORT, OnSendReport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ErrorReport message handlers

BOOL ErrorReport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ErrorReport::OnSendReport() 
{
	// TODO: Add your control notification handler code here
	CString s;
	s.Format(_T("mailto:?subject=error report&body=%s"),report);
  HINSTANCE h = ShellExecute(NULL, _T("open"), s, NULL, NULL, SW_SHOWNORMAL);
  ASSERT((UINT)h > 32);	
}
