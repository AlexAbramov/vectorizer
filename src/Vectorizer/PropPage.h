#pragma once

#include "Param.h"

// PropPage dialog

class PropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(PropPage)

public:
	PropPage();
	virtual ~PropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_LARGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  TransformMethod transformMethod;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
private:
	double maxDev;
	double mergeFactor;
	double maxGapDev;
	double maxGapLength;
	BOOL colorDetection;
public:
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedCheck1();
};
