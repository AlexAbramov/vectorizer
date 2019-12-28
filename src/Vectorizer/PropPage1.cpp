// PropPage1.cpp : implementation file
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "PropPage1.h"


// PropPage1 dialog

IMPLEMENT_DYNAMIC(PropPage1, CPropertyPage)
PropPage1::PropPage1()
	: CPropertyPage(PropPage1::IDD)
	, maxSpotAreaFilter(FALSE)
	, minPolylineLengthFilter(FALSE)
	, maxPolylineLengthFilter(FALSE)
	, maxPolylineLength(0)
	, minPolylineLength(0)
	, maxSpotArea(0)
{
}

PropPage1::~PropPage1()
{
}

void PropPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, maxSpotAreaFilter);
	DDX_Check(pDX, IDC_CHECK3, minPolylineLengthFilter);
	DDX_Check(pDX, IDC_CHECK4, maxPolylineLengthFilter);
	DDX_Text(pDX, IDC_EDIT6, maxPolylineLength);
	DDV_MinMaxDouble(pDX, maxPolylineLength, 0.0, 100000.0);
	DDX_Text(pDX, IDC_EDIT3, minPolylineLength);
	DDV_MinMaxDouble(pDX, minPolylineLength, 0.0, 100000.0);
	DDX_Text(pDX, IDC_EDIT5, maxSpotArea);
	DDV_MinMaxInt(pDX, maxSpotArea, 0, 100000);
}


BEGIN_MESSAGE_MAP(PropPage1, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnBnClickedCheck4)
	ON_EN_CHANGE(IDC_EDIT5, OnEnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT6, OnEnChangeEdit6)
END_MESSAGE_MAP()


// PropPage1 message handlers

void PropPage1::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	this->GetDlgItem(IDC_EDIT5)->EnableWindow(maxSpotAreaFilter);
}

void PropPage1::OnBnClickedCheck3()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	this->GetDlgItem(IDC_EDIT3)->EnableWindow(minPolylineLengthFilter);
}

void PropPage1::OnBnClickedCheck4()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	this->GetDlgItem(IDC_EDIT6)->EnableWindow(maxPolylineLengthFilter);
}

BOOL PropPage1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	maxPolylineLength=param.maxPolylineLength;
	minPolylineLength=param.minPolylineLength;
	maxSpotArea=param.maxSpotArea;

	maxSpotAreaFilter=param.maxSpotAreaFilter;
	minPolylineLengthFilter=param.minPolylineLengthFilter;
	maxPolylineLengthFilter=param.maxPolylineLengthFilter;

	this->GetDlgItem(IDC_EDIT5)->EnableWindow(maxSpotAreaFilter);
	this->GetDlgItem(IDC_EDIT3)->EnableWindow(minPolylineLengthFilter);
	this->GetDlgItem(IDC_EDIT6)->EnableWindow(maxPolylineLengthFilter);

  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PropPage1::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	param.maxPolylineLengthFilter=maxPolylineLength;
	param.minPolylineLength=minPolylineLength;
	param.maxSpotArea=maxSpotArea;

	param.maxSpotAreaFilter=maxSpotAreaFilter;
	param.minPolylineLengthFilter=minPolylineLengthFilter;
	param.maxPolylineLengthFilter=maxPolylineLengthFilter;
	
	param.Update();

	CPropertyPage::OnOK();
}

void PropPage1::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SetModified();
}

void PropPage1::OnEnChangeEdit3()
{
	// TODO:  Add your control notification handler code here
	SetModified();
}

void PropPage1::OnEnChangeEdit6()
{
	// TODO:  Add your control notification handler code here
	SetModified();
}
