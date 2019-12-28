#pragma once

// StaticLink.h : header file
//

// StaticLink implements a static control that's a hyperlink
 // to any file on your desktop or web. You can use it in dialog boxes
 // to create hyperlinks to web sites. When clicked, opens the file/URL
 //
 class StaticLink : public CStatic {
 public:
     StaticLink();
 
     // you can change these any time:
     COLORREF     m_colorUnvisited;         // color for unvisited
     COLORREF     m_colorVisited;           // color for visited
     BOOL         m_bVisited;               // whether visited or not
 
     // URL/filename for non-text controls (e.g., icon, bitmap) or when link is
     // different from window text. If you don't set this, CStaticIcon will
     // use GetWindowText to get the link.
     CString m_link;
 
 protected:
     DECLARE_DYNAMIC(StaticLink)
     CFont         m_font;                  // underline font for text control
 
     // message handlers
     afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
     afx_msg void    OnClicked();
     DECLARE_MESSAGE_MAP()
};