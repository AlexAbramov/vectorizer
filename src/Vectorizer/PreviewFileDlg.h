#pragma once

#include "ImageStatic.h"

/////////////////////////////////////////////////////////////////////////////
// PreviewFileDlg dialog

class PreviewFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(PreviewFileDlg)

public:
	ImageStatic imageStatic;
	PreviewFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = 0,//OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	bool bPreview;
	//{{AFX_MSG(PreviewFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck1();
	//}}AFX_MSG
  virtual void OnFileNameChange();
	DECLARE_MESSAGE_MAP()
};
