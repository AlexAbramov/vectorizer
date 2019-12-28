#pragma once
#include "StaticLink.h"

class GPDialog : public CDialog
{
	DECLARE_DYNAMIC(GPDialog)
	HCURSOR curLink;
public:
	GPDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~GPDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	StaticLink url;
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
