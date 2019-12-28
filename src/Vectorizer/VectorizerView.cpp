// VectorizerView.cpp : implementation of the CVectorizerView class
//

#include "stdafx.h"
#include "Vectorizer.h"
#include "..\CommonLib\clresource.h"
#include "VectorizerDoc.h"
#include "VectorizerView.h"

////////////////////////////////////////
//  panning helper class

class CHandTracker : public CRectTracker
{
    CVectorizerView *m_pView;
public:
    CHandTracker(CVectorizerView *pView);
    virtual void OnChangedRect(const CRect& rectOld);
    virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
	{ /* no-op ! */ }
};


CHandTracker::CHandTracker(CVectorizerView *pView)
{
    ASSERT(pView != NULL);
    m_pView = pView;
    m_sizeMin = CSize(1, 1);
}


void CHandTracker::OnChangedRect(const CRect& rectOld)
{
    CPoint delta;
    // call the view to update scroll position
    delta.x = m_rect.Width()  - rectOld.Width();
    delta.y = m_rect.Height() - rectOld.Height();
    // m_pView->ViewDPtoLP( &delta );
    CPoint anchor = m_pView->GetScrollPosition();
    anchor -= delta;

    if (anchor.x < 0) anchor.x = 0;
    if (anchor.y < 0) anchor.y = 0;
    if (! (m_pView->GetStyle() & WS_VSCROLL)) anchor.x = 0;
    if (! (m_pView->GetStyle() & WS_HSCROLL)) anchor.x = 0;

    if ( delta.x || delta.y )
	m_pView->ScrollToPosition( anchor );
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerView

IMPLEMENT_DYNCREATE(CVectorizerView, ZoomView)

BEGIN_MESSAGE_MAP(CVectorizerView, ZoomView)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_FITIMAGE, OnFitImage)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
//	ON_COMMAND(ID_VIEW_ZOOM_MODE, OnZoomMode)
	ON_WM_MOUSEWHEEL()
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, ZoomView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, ZoomView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, ZoomView::OnFilePrintPreview)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorizerView construction/destruction

CVectorizerView::CVectorizerView()
{
	// TODO: add construction code here
  bZooming = false;
	
  curPoint = AfxGetApp()->LoadCursor(IDC_POINTER);
  curMove  = AfxGetApp()->LoadCursor(IDC_HMOVE);
}

CVectorizerView::~CVectorizerView()
{
  if(curPoint) DestroyCursor(curPoint);
  if(curMove) DestroyCursor(curMove );
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerView drawing

void CVectorizerView::OnDraw(CDC* pDC)
{
	CVectorizerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
//	pDC->Rectangle(100,100,200,200);

	// TODO: add draw code for native data here
  CSize docsize;
	if(!pDoc->GetSize(docsize)) return;
  CRect rcImg(0,0,docsize.cx,docsize.cy);
  CRect rcDest;
  if (! pDC->IsPrinting())   // display DC
  {
		rcDest = rcImg = invalidRect;
  }
  else  // printer device context
  {
    // get size of printer page (in pixels)
    int cxPage = pDC->GetDeviceCaps(HORZRES);
    int cyPage = pDC->GetDeviceCaps(VERTRES);
    // get printer pixels per inch
    int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
    int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

    // create a rectangle which preserves the document's aspect ratio,
    // and fills the page taking into account the relative size of a
    // printed pixel (cyInch / cxInch).

    rcDest.top = rcDest.left = 0;
    rcDest.right = cxPage;
    rcDest.bottom = MulDiv(docsize.cy * cyInch, cxPage, docsize.cx * cxInch);
    if (rcDest.bottom > cyPage)
    {
      rcDest.bottom = cyPage;
      rcDest.right  = MulDiv(docsize.cx * cxInch, cyPage, docsize.cy * cyInch);
    }
  }

	if(rcDest.IsRectEmpty()) return;

	bool needClear=pDoc->ext=="wmf"||!pDoc->viewRaster;
	if(needClear)
	{
//		CBrush brush
		pDC->FillRect(rcDest,0);
	}

	CImage* image=pDoc->image;
	if(image&&!image->IsNull()&&pDoc->viewRaster)
	{
		image->Draw(*pDC,rcDest,rcImg);
	}
	double zr=this->GetZoomRatio();
  if (pDC->IsPrinting())
	{
		if(docsize.cx<=0) return;
		zr=double(rcDest.Width())/docsize.cx;
		rcDest.SetRect(0,0,docsize.cx,docsize.cy);
	}
	if(pDoc->viewVector) pDoc->vectorizer.Draw(*pDC,rcDest,zr);
}

void CVectorizerView::OnInitialUpdate()
{
	ZoomView::OnInitialUpdate();

	// TODO: calculate the total size of this view
	CVectorizerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

  // set scrolling ranges
  NotifyRanges();

  // size window perfectly around frame
  GetParentFrame()->RecalcLayout();
//  OnSizeToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerView printing

BOOL CVectorizerView::OnPreparePrinting(CPrintInfo* pInfo)
{
    pInfo->SetMaxPage(1);
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CVectorizerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVectorizerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVectorizerView diagnostics

#ifdef _DEBUG
void CVectorizerView::AssertValid() const
{
	ZoomView::AssertValid();
}

void CVectorizerView::Dump(CDumpContext& dc) const
{
	ZoomView::Dump(dc);
}

CVectorizerDoc* CVectorizerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVectorizerDoc)));
	return (CVectorizerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVectorizerView message handlers

void CVectorizerView::NotifyRanges()
{
	CSize size;
	GetDocument()->GetSize(size);
  SetZoomSizes(size);
  SetZoomMode( MODE_ZOOMOFF );
  bZooming = false;
  SetZoomRatio(1.0);	// not adjusted by CZoomView ?!?
}

BOOL CVectorizerView::IsMovable() const
{
  return (GetStyle() & (WS_VSCROLL | WS_HSCROLL));
}

void CVectorizerView::OnSizeToFit()
{
  ResizeParentToFit(false);

  // MFC might have grown the window off the screen--I'll fix it
  CRect rc, rcMax;
  CFrameWnd* pFrame = GetParentFrame();
  ASSERT_VALID(pFrame);
  pFrame->GetWindowRect(&rc);
  CWnd* pGrandParent = pFrame->GetParent();
  if (pGrandParent)
  {
      // use top level window as maximum rectangle
      pGrandParent->ScreenToClient(&rc);
      pGrandParent->GetClientRect(&rcMax);
  }
  else
  {
      // use whole screen as maximum rectangle
      rcMax.SetRect(0,0,GetSystemMetrics(SM_CXSCREEN),
                    GetSystemMetrics(SM_CYSCREEN));
  }

  bool bTooBig = false;
  if (rc.bottom > rcMax.bottom)
  {
      rc.bottom = rcMax.bottom;
      rc.right += GetSystemMetrics(SM_CXVSCROLL) +
                  GetSystemMetrics(SM_CXBORDER);
      bTooBig = true;
  }
  if (rc.right > rcMax.right)
  {
      rc.right = rcMax.right;
      bTooBig = true;
  }
  if (bTooBig)
  {
      pFrame->SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(),
                           SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
  }
}

void CVectorizerView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
  ZoomView::OnActivateView(bActivate, pActivateView, pDeactiveView);

  if (bActivate)
  {
      // probably we switched from view/doc within MDI app,
      // so this view deserves foreground palette
      ASSERT(pActivateView == this);
//      OnDoRealize((WPARAM)m_hWnd, 0);    //
			// same as SendMessage(WM_DOREALIZE);
  }
}

void CVectorizerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
    if ( ! pDC->IsPrinting())
	ZoomView::OnPrepareDC( pDC, pInfo );
    else
	CScrollView::OnPrepareDC( pDC, pInfo );
}

BOOL CVectorizerView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
  if ((GetStyle() & WS_VSCROLL) && (GetStyle() & WS_HSCROLL))  // page occupies whole view
	  return true;
  else return ZoomView::OnEraseBkgnd(pDC);
}

void CVectorizerView::OnFitImage() 
{
	DoZoomFull();
}

void CVectorizerView::OnPaint() 
{
  if ( GetUpdateRect( invalidRect ))
  {
	  invalidRect.NormalizeRect();
	// TRACE_RECT( invalidRect );
		ViewDPtoLP( (LPPOINT) &invalidRect, 2 );
		CSize size;
		GetDocument()->GetSize(size);
		CRect docrect( CPoint(0,0), size);
		invalidRect.InflateRect( 1, 1 );
		invalidRect &= docrect;
  }
  else invalidRect.SetRect( 0,0,0,0 );

  ZoomView::OnPaint();
}

BOOL CVectorizerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if (nHitTest != HTCLIENT)
        nDocPosX = nDocPosY = -1;

    //    if (GetZoomMode() == MODE_ZOOMOFF && nHitTest == HTCLIENT && IsMovable() )
    if (! bZooming && nHitTest == HTCLIENT && IsMovable() )
    {
        ::SetCursor(curPoint);
        return true;
    }
    else
        return ZoomView::OnSetCursor(pWnd, nHitTest, message);
}

void CVectorizerView::OnSize(UINT nType, int cx, int cy) 
{
  ZoomView::OnSize(nType, cx, cy);
}

void CVectorizerView::OnZoomMode()
{
  bZooming = ! bZooming;
  SetZoomMode(bZooming ? MODE_ZOOMIN : MODE_ZOOMOFF);
}

void CVectorizerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  switch( nChar )
  {
  case VK_HOME:
      SendMessage(::GetAsyncKeyState(VK_CONTROL) ?
                  WM_VSCROLL : WM_HSCROLL,SB_TOP);
      break;
  case VK_END:
      SendMessage(::GetAsyncKeyState(VK_CONTROL) ?
                  WM_VSCROLL : WM_HSCROLL,SB_BOTTOM);
      break;
  case VK_PRIOR:
      SendMessage(WM_VSCROLL,SB_PAGEUP);
      break;
  case VK_NEXT:
      SendMessage(WM_VSCROLL,SB_PAGEDOWN);
      break;
  case VK_UP:
      SendMessage(WM_VSCROLL,SB_LINEUP);
      break;
  case VK_DOWN:
      SendMessage(WM_VSCROLL,SB_LINEDOWN);
      break;
  case VK_LEFT:
      SendMessage(WM_HSCROLL,SB_LINEUP);
      break;
  case VK_RIGHT:
      SendMessage(WM_HSCROLL,SB_LINEDOWN);
      break;
  default:
      ZoomView::OnKeyDown(nChar, nRepCnt, nFlags);
  }
}

void CVectorizerView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // if (GetZoomMode() == MODE_ZOOMOFF && IsMovable())
  if (!bZooming && IsMovable())
  {
	  HCURSOR prvCur = ::SetCursor(curMove);

	  if(CHandTracker(this).TrackRubberBand(this, point))
	    Invalidate(false);

	  ::SetCursor(prvCur);
  }
  else if(bZooming)
  {
	// emulate 3-button mouse with <Ctrl> key
	  if( nFlags & MK_CONTROL )
	    OnMButtonDown(nFlags, point);
	  else
	    ZoomView::OnLButtonDown(nFlags, point);
  }
}

void CVectorizerView::OnMButtonDown(UINT nFlags, CPoint point) 
{
  if (GetZoomMode() != MODE_ZOOMOFF)
	DoZoomFull();
#ifdef OWND
    else if ( IsMovable() )
	StartPanning( this, GetStyle() & WS_VSCROLL, GetStyle() & WS_HSCROLL, point );
#endif	// OWND
}

void CVectorizerView::OnMouseMove(UINT nFlags, CPoint point) 
{
    CPoint pt( point );
    ViewDPtoLP( &pt );
    CSize docsize;
		GetDocument()->GetSize(docsize);
    if ( pt.x >= 0 && pt.x < docsize.cx
	    && pt.y >= 0 && pt.y < docsize.cy )
    {
        nDocPosX = pt.x;
        nDocPosY = pt.y;
    }
    else
        nDocPosX = nDocPosY = -1;

    ZoomView::OnMouseMove(nFlags, point);
}

void CVectorizerView::OnRButtonDown(UINT nFlags, CPoint point) 
{
/*  if (GetZoomMode() == MODE_ZOOMOFF) 
		NotifyRanges();
  else
  {
		BOOL bWasZoomed = IsMovable();
		ViewDPtoLP( &point );
		DoZoomOut( &point );
  }*/
}

BOOL CVectorizerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	//Special logic for CONTROL and mousewheel which is zooming.
	if (nFlags == MK_CONTROL)
	{
		//int zoom = GetZoom();
//		float fCurrentZoom = m_fZoomScale;
		if (zDelta > 0)
			DoZoomIn(0);//&pt
		else 
			DoZoomOut(0);//!!!
	}
	else if (nFlags == MK_SHIFT)
	{
		//Here you would implement your data traversal routines.
		//Unfortunately this sample doesn't have any, so let's
		//just display a nice debug output message instead.

		//TRACE("Got a shift key!\n");
	}
	else return ZoomView::OnMouseWheel(nFlags, zDelta, pt);
	return true;
}
void CVectorizerView::OnViewZoomin()
{
	// TODO: Add your command handler code here
	this->DoZoomIn();
}

void CVectorizerView::OnViewZoomout()
{
	// TODO: Add your command handler code here
	this->DoZoomOut();
}
