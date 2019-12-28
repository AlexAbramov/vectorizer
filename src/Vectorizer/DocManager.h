#pragma once

class DocManager: public CDocManager
{
  DECLARE_DYNAMIC(DocManager)
public:
	CImage* image;
	DocManager();
	virtual ~DocManager();
  virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
};
