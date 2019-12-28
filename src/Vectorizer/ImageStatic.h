#pragma once

class ImageStatic : public CStatic
{
public:
	CString path;
	CImage* image;

// Attributes
public:

// Operations
public:
	ImageStatic();
	virtual ~ImageStatic();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PictureStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Unload();
	bool Load(LPCTSTR fileName);

	// Generated message map functions
protected:
	void Paint();
	//{{AFX_MSG(PictureStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};