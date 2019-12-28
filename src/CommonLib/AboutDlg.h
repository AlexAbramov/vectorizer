#pragma once
#include "StaticLink.h"

class AboutDlg : public CDialog
{
	HCURSOR curLink;
public:
	AboutDlg();
	~AboutDlg();

	StaticLink url;
	StaticLink email;
	StaticLink logo;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
};
