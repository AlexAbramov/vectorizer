#pragma once
#include "Protect.h"
#include "StaticLink.h"

class RegistrationDialog : public CDialog
{
	HCURSOR curLink;
// Construction
public:
	RegistrationDialog(CWnd* pParent = NULL);   // standard constructor
	CString GetKey(){return key;}
private:
	CString regid;
	StaticLink	regSite;
	CButton	btnOk;
	CString	key;
	CString	regNum;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeKey();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
};
