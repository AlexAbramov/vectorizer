#if !defined(AFX_ROTATINGGLOBE_H__8FF71770_D304_4AD1_AF03_2AB6B889F0EE__INCLUDED_)
#define AFX_ROTATINGGLOBE_H__8FF71770_D304_4AD1_AF03_2AB6B889F0EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RotatingGlobe.h : header file
//

//#include "StaticLink.h"

/////////////////////////////////////////////////////////////////////////////
// RotatingGlobe window

class SPolygons;

class RotatingGlobe : public CStatic
{
private:
	int angle;
	int dAngle;
	SPolygons* polygons;
  void Paint();
	bool hasTimer;
public:
	RotatingGlobe();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RotatingGlobe)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RotatingGlobe();

	// Generated message map functions
protected:
	//{{AFX_MSG(RotatingGlobe)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATINGGLOBE_H__8FF71770_D304_4AD1_AF03_2AB6B889F0EE__INCLUDED_)
