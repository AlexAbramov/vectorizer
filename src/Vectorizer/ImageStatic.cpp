// ImageStatic.cpp : implementation file
//

#include "stdafx.h"
#include "vectorizer.h"
#include "ImageStatic.h"

/////////////////////////////////////////////////////////////////////////////
// ImageStatic

ImageStatic::ImageStatic(): image(0)
{
}

ImageStatic::~ImageStatic()
{
	if(image) delete image;
}


BEGIN_MESSAGE_MAP(ImageStatic, CStatic)
	//{{AFX_MSG_MAP(ImageStatic)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ImageStatic message handlers

bool ImageStatic::Load(LPCTSTR fileName)
{
	Unload();
	if(image==0) image=new CImage();
	bool res=SUCCEEDED(image->Load(fileName));
	if(res)
	{
		path=fileName;
		Paint();
	}
	return res;
}

void ImageStatic::Unload()
{
	path.Empty();
  if(image&&!image->IsNull())
	{
		image->Destroy();
    Paint();
	}
}

void ImageStatic::Paint()
{
	if (this->m_hWnd) {
		CRect rect;
		GetClientRect(&rect);
		ASSERT(rect.Height()==rect.Width());
		CClientDC dc(this);

		CBrush brush(GetSysColor(COLOR_3DFACE));
		dc.FillRect(rect, &brush);

		if (!IsValidImage(image))	return;

		CSize sizeSrc(image->GetWidth(), image->GetHeight());
		CRect rectSrc(CPoint(0, 0), sizeSrc);

		int d = rect.Width();
		if (sizeSrc.cx > sizeSrc.cy)
		{
			int h = MulDiv(d, sizeSrc.cy, sizeSrc.cx);
			rect.InflateRect(0, (h - d) / 2);
		}
		else
		{
			int w = MulDiv(d, sizeSrc.cx, sizeSrc.cy);
			rect.InflateRect((w - d) / 2, 0);
		}
		image->Draw(dc, rect, rectSrc);
	}
}

HBRUSH ImageStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	Paint();
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}
