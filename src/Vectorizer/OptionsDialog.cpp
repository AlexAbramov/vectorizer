// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "OptionsDialog.h"


// OptionsDialog

IMPLEMENT_DYNAMIC(OptionsDialog, CPropertySheet)
OptionsDialog::OptionsDialog(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Init();
}

OptionsDialog::OptionsDialog(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Init();
}

OptionsDialog::~OptionsDialog()
{
}


BEGIN_MESSAGE_MAP(OptionsDialog, CPropertySheet)
END_MESSAGE_MAP()


// OptionsDialog message handlers

void OptionsDialog::Init(void)
{
	AddPage(&pp);
	AddPage(&pp1);
}
