#pragma once

#include "Batch.h"

class CVectorizerDoc : public CDocument
{
	friend CVectorizerApp;
	bool AddOption(CString& key, CString& value);
	bool SetOptions(LPCTSTR options);
	CString GetOptions();
	bool demo,bTrue;
protected: // create from serialization only
	CVectorizerDoc();
	DECLARE_DYNCREATE(CVectorizerDoc)
// Attributes
public:
	bool IsDemo(){return demo;}
  CString ext,outExt;
	CImage* image;
	Vectorizer vectorizer;
	bool GetSize(SIZE& size);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorizerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVectorizerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVectorizerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CVectorizerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	bool viewRaster;
	bool viewVector;
	afx_msg void OnViewRaster();
	afx_msg void OnViewVector();
	afx_msg void OnUpdateViewRaster(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewVector(CCmdUI *pCmdUI);
	afx_msg void OnVectorize();
	afx_msg void OnOptions();
protected:
	VARIANT_BOOL Open(LPCTSTR pathName);

	enum 
	{
		dispidGetVersion = 6L,		dispidGetOptions = 5L,		dispidSetOptions = 4L,		dispidVectorize = 3L,		dispidSave = 2L,		dispidOpen = 1L
	};
	VARIANT_BOOL Save(LPCTSTR pathName);
	VARIANT_BOOL Vectorize(void);
	VARIANT_BOOL SetOptions1(LPCTSTR options);
	BSTR GetOptions1(void);
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVectorize(CCmdUI *pCmdUI);
protected:
	BSTR GetVersion(void);
};
