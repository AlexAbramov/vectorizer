#pragma once

#include "Param.h"

// PropPage1 dialog

class PropPage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(PropPage1)

public:
	PropPage1();
	virtual ~PropPage1();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_LARGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck1();
private:
	BOOL maxSpotAreaFilter;
	BOOL minPolylineLengthFilter;
	BOOL maxPolylineLengthFilter;
	double maxPolylineLength;
	double minPolylineLength;
	int maxSpotArea;
public:
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit6();
};
