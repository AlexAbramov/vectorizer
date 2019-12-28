#pragma once

#include "ZoomView.h"

class CVectorizerView : public ZoomView
{
  bool bZooming;
  HCURSOR curMove;
  HCURSOR curPoint;
  HCURSOR curZoom;
  CRect invalidRect;
  int nDocPosX;
  int	nDocPosY;
protected: // create from serialization only
	CVectorizerView();
	DECLARE_DYNCREATE(CVectorizerView)

// Attributes
public:
	CVectorizerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorizerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVectorizerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVectorizerView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFitImage();
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnZoomMode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void NotifyRanges();
	void OnSizeToFit();
  BOOL IsMovable() const;
public:
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
};

#ifndef _DEBUG  // debug version in VectorizerView.cpp
inline CVectorizerDoc* CVectorizerView::GetDocument()
   { return (CVectorizerDoc*)m_pDocument; }
#endif
