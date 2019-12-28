#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "..\CommonLib\CommonLib.h"

/////////////////////////////////////////////////////////////////////////////
// CVectorizerApp:
// See Vectorizer.cpp for the implementation of this class
//

class DocManager;

class CVectorizerApp : public CWinApp
{
	bool runAutomated;
	CStringList rasterExt;
	CStringList vectorExt;
	bool HasExt(LPCTSTR ext,CStringList& slist);
	bool demo;
public:
	bool IsDemo()const;
	DocManager* GetDocManager(){return (DocManager*)m_pDocManager;}
	bool IsRasterExt(LPCTSTR ext){return HasExt(ext,rasterExt);}
	bool IsVectorExt(LPCTSTR ext){return HasExt(ext,vectorExt);}
	CVectorizerApp();
	bool RunAutomated(){return runAutomated;}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorizerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation
	//{{AFX_MSG(CVectorizerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void DoBatch(void);
public:
	virtual int ExitInstance();
};

inline bool IsValidImage(CImage* image){return image&&!image->IsNull()&&image->GetWidth()>0&&image->GetHeight()>0;}
inline bool Intersects(const RECT& r1,const RECT& r2){return r1.right>=r2.left&&r2.right>=r1.left&&r1.bottom>=r2.top&&r2.bottom>=r1.top;}

extern CVectorizerApp theApp;

void Message(LPCTSTR s);