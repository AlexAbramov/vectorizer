#pragma once

#include "PropPage.h"
#include "PropPage1.h"

// OptionsDialog

class OptionsDialog : public CPropertySheet
{
	PropPage pp;
	PropPage1 pp1;
	DECLARE_DYNAMIC(OptionsDialog)

public:
	OptionsDialog(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	OptionsDialog(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~OptionsDialog();

protected:
	DECLARE_MESSAGE_MAP()
private:
	void Init(void);
};