// PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "PropPage.h"


// PropPage dialog

IMPLEMENT_DYNAMIC(PropPage, CPropertyPage)
PropPage::PropPage()
	: CPropertyPage(PropPage::IDD)
	, maxDev(0)
	, mergeFactor(0)
	, maxGapDev(0)
	, maxGapLength(0)
	, colorDetection(FALSE)
{
}

PropPage::~PropPage()
{
}

void PropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, maxDev);
	DDV_MinMaxDouble(pDX, maxDev, -100.0, 100.0);
	DDX_Text(pDX, IDC_EDIT3, mergeFactor);
	DDV_MinMaxDouble(pDX, mergeFactor, 0.0, 100.0);
	DDX_Text(pDX, IDC_EDIT2, maxGapDev);
	DDV_MinMaxDouble(pDX, maxGapDev, 0.0, 100.0);
	DDX_Text(pDX, IDC_EDIT4, maxGapLength);
	DDV_MinMaxDouble(pDX, maxGapLength, 0.0, 1000.0);
	DDX_Check(pDX, IDC_CHECK1, colorDetection);
}

BEGIN_MESSAGE_MAP(PropPage, CPropertyPage)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()


// PropPage message handlers

void PropPage::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here	
	if(transformMethod!=tmCenterLine)
	{
    transformMethod=tmCenterLine;
		this->SetModified();
	}
}

void PropPage::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	if(transformMethod!=tmInsideContour)
	{
    transformMethod=tmInsideContour;
		this->SetModified();
	}
}

void PropPage::OnBnClickedRadio3()
{
	// TODO: Add your control notification handler code here
	if(transformMethod!=tmOutsideContour)
	{
    transformMethod=tmOutsideContour;
		this->SetModified();
	}
}

BOOL PropPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	transformMethod=param.transformMethod;
	switch(transformMethod)
	{
	case tmCenterLine:
		this->CheckDlgButton(IDC_RADIO1,BST_CHECKED);
		break;
	case tmInsideContour:
		this->CheckDlgButton(IDC_RADIO2,BST_CHECKED);
		break;
	case tmOutsideContour:
		this->CheckDlgButton(IDC_RADIO3,BST_CHECKED);
		break;
	}
  maxDev=param.maxDev;
	mergeFactor=param.mergeFactor;
	maxGapDev=param.maxGapDev;
	maxGapLength=param.maxGapLength;
	colorDetection=param.colorDetection;
  UpdateData(FALSE);
	GetDlgItem(IDC_EDIT2)->EnableWindow(mergeFactor>0.0);
	GetDlgItem(IDC_EDIT4)->EnableWindow(mergeFactor>0.0);
	GetDlgItem(IDC_EDIT5)->SetWindowText("It's recommended to vectorize images with little number of colors with this option.");
	GetDlgItem(IDC_EDIT5)->ShowWindow(colorDetection?SW_SHOW:SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PropPage::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertyPage::OnOK();
  param.transformMethod=transformMethod;
	param.maxDev=maxDev;
	param.colorDetection=colorDetection;
	param.mergeFactor=mergeFactor;
	param.maxGapDev=maxGapDev;
	param.maxGapLength=maxGapLength;
	param.Update();
}

void PropPage::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SetModified();
	UpdateData();
	this->GetDlgItem(IDC_EDIT2)->EnableWindow(mergeFactor>0.0);
	this->GetDlgItem(IDC_EDIT4)->EnableWindow(mergeFactor>0.0);
}

void PropPage::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	this->GetDlgItem(IDC_EDIT5)->ShowWindow(colorDetection?SW_SHOW:SW_HIDE);
}
