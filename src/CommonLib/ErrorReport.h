#pragma once

void ReportError(LPCTSTR report);

class ErrorReport : public CDialog
{
// Construction
public:
	ErrorReport(LPCTSTR report,CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_ERRORREPORT };
	CString	report;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnSendReport();
	DECLARE_MESSAGE_MAP()
};
