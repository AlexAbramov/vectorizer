// StaticLink.cpp : implementation file
//

#include "stdafx.h"
#include "StaticLink.h"

IMPLEMENT_DYNAMIC(StaticLink, CStatic)
 
BEGIN_MESSAGE_MAP(StaticLink, CStatic)
   ON_WM_CTLCOLOR_REFLECT()
   ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
END_MESSAGE_MAP()
///////////////////
// Constructor sets default colors = blue/purple.
//
StaticLink::StaticLink()
{
  m_colorUnvisited = RGB(0,0,255);       // blue
  m_colorVisited   = RGB(128,0,128);     // purple
  m_bVisited       = FALSE;              // not visited yet
}
//////////////////// Handle reflected WM_CTLCOLOR to set custom control color.
// For a text control, use visited/unvisited colors and underline font.
// For non-text controls, do nothing. Also ensures SS_NOTIFY is on.
//
HBRUSH StaticLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
  ASSERT(nCtlColor == CTLCOLOR_STATIC);
  DWORD dwStyle = GetStyle();
  if (!(dwStyle & SS_NOTIFY)) {
       // Turn on notify flag to get mouse messages and STN_CLICKED.
       // Otherwise, I'll never get any mouse clicks!
       ::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
  }
   
  HBRUSH hbr = NULL;
  if ((dwStyle & 0xFF) <= SS_RIGHT) {

       // this is a text control: set up font and colors
       if (!(HFONT)m_font) {
           // first time init: create font
           LOGFONT lf;
           GetFont()->GetObject(sizeof(lf), &lf);
           lf.lfUnderline = TRUE;
           m_font.CreateFontIndirect(&lf);
       }

       // use underline font and visited/unvisited colors
       pDC->SelectObject(&m_font);
       pDC->SetTextColor(m_bVisited ? m_colorVisited : m_colorUnvisited);
       pDC->SetBkMode(TRANSPARENT);

       // return hollow brush to preserve parent background color
       hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
   }
   return hbr;
}
/////////////////
// Handle mouse click: open URL/file.
//
void StaticLink::OnClicked()
{
	if(m_link.IsEmpty()) GetWindowText(m_link);

	const LPCTSTR prefix=_T("mailto:");
	bool needPrefix=m_link.Find(_T('@'))!=-1 && m_link.Find(prefix)==-1;
	CString link=needPrefix ? prefix + m_link : m_link;

  HINSTANCE h = ShellExecute(NULL, _T("open"), link, NULL, NULL, SW_SHOWNORMAL);
  if ((UINT)h > 32) 
	{
    m_bVisited = TRUE;       // (not really--might not have found link)
    Invalidate();            // repaint to show visited color
  } 
	else 
	{
    ASSERT(0);
  }
}