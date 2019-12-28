// PreviewFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "PreviewFileDlg.h"

/////////////////////////////////////////////////////////////////////////////
// PreviewFileDlg

IMPLEMENT_DYNAMIC(PreviewFileDlg, CFileDialog)

PreviewFileDlg::PreviewFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
  m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE);
  m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILEOPENPREVIEW);
	bPreview = true;
}


BEGIN_MESSAGE_MAP(PreviewFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(PreviewFileDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL PreviewFileDlg::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  GetDlgItem(IDC_CHECK1)->SendMessage(BM_SETCHECK, bPreview);
  imageStatic.SubclassDlgItem(IDC_IMAGE, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PreviewFileDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	bPreview=!bPreview;
  if(bPreview) OnFileNameChange();
	else imageStatic.Unload();
}

void PreviewFileDlg::OnFileNameChange()
{
  CFileDialog::OnFileNameChange();
  if(bPreview)
	{
		CString ext=GetFileExt();
		if(theApp.IsRasterExt(ext))
      imageStatic.Load(GetPathName());
	}
}