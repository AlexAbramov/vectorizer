// Batch.cpp: implementation of the Batch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vectorizer.h"
#include "batch.h"
#include "Param.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void Batch::Clear()
{
	POSITION pos=polylines.GetHeadPosition();
	while(pos)
	{
	  PolyLine* pline =polylines.GetNext(pos);
		delete pline;
	}
	polylines.RemoveAll();
	if(buf)
	{
		delete []buf;
	  buf=0;
	}
	bufsize=0;
	plinePos=0;
}

POINT* Batch::NextPolyline(int& _count)
{
	if(!plinePos)
	{
	  plinePos=polylines.GetHeadPosition();
	  _count=0;
	  return 0;
	}
	PolyLine* pline=polylines.GetNext(plinePos);
	int count=pline->Count();
	if(bufsize<count)
	{
		bufsize=count;
		delete[] buf;
		buf=new POINT[bufsize];
	}
	POSITION pos=pline->GetHeadPosition();
	for(int i=0;i<count;i++) buf[i]=pline->GetNext(pos);
	_count=count;
	return buf;
}

bool Vectorizer::IsDemo()
{
	return batch.IsDemo();
}

void Batch::Draw(CDC& dc,CRect& rect,double& zr)
{
	if(Count()<=0) return;
	CPen* pen=0;
	if(!param.colorDetection)
	{
    pen=new CPen(PS_SOLID,1,RGB(0,255,255));
		dc.SelectObject(pen);
	}
	
	CRect r;	
	POSITION pos=polylines.GetHeadPosition(); 
	while(pos)
	{
	  PolyLine* pline =(PolyLine*)polylines.GetNext(pos);
		if(pline->GetBoundRect(r)&&Intersects(r,rect))
		{
    	if(param.colorDetection)
			{
				if(pen) delete pen;
				pen=new CPen(PS_SOLID,1,pline->GetColor());
				dc.SelectObject(pen);
			}
			pline->Draw(dc,zr);
		}
	}
	if(pen) delete pen;
}

void Batch::Merge(Progress* pr)
{
	plinePos=0;
	int count0=polylines.GetCount();
	if(pr) pr->Range(count0);
	for(POSITION pos=polylines.GetHeadPosition();pos;polylines.GetNext(pos))
	{
		while(Merge(pos,true));
		while(Merge(pos,false));
		if(pr)
		{
			int count=polylines.GetCount();
			if(!pr->Position(count0-count)) break; 
			count0++;
		}
	}
}

bool Batch::Merge(POSITION pos,bool head)
{
	PolyLine* pline=polylines.GetNext(pos);
	Point& p=pline->HeadOrTail(head);
	POSITION targetPos=0;
	bool targetHead;
	UINT targetDsq;
	PolyLine* pline1=0;
	bool head1=true;
	while(pos)
	{
		if(head1) pline1=polylines.GetAt(pos);
		UINT dsq=p.DistanceSq(pline1->HeadOrTail(head1));
		if(dsq<param.maxGapLengthSq)
		{
			if(pline->CanMerge(head,*pline1,head1))
			{
				if(!targetPos||dsq<targetDsq)
				{
					targetPos=pos;
					targetDsq=dsq;
					targetHead=head1;
				}
			}
		}
		head1=!head1;
		if(head1) polylines.GetNext(pos);
	}
	if(targetPos)
	{
		pline1=polylines.GetAt(targetPos);
		pline->Merge(head,*pline1,targetHead);
		polylines.RemoveAt(targetPos);
		delete pline1;
		return true;
	}
	return false;
}

void Batch::Add(PolyLine* pline)
{
	ASSERT(pline&&pline->Count()>=2);
	polylines.AddTail(pline);
}

void Batch::Filter(Progress* pr)
{
	if(!param.minPolylineLengthFilter&&!param.maxPolylineLengthFilter) return;
	plinePos=0;
	int count0=polylines.GetCount();
	POSITION pos=polylines.GetHeadPosition();
	while(pos)
	{
		POSITION pos0=pos;
	  PolyLine* pline =polylines.GetNext(pos);
		double l=pline->Length();
		bool del=false;
		if(param.minPolylineLengthFilter&&l<param.minPolylineLength) del=true;
		else if(param.maxPolylineLengthFilter&&l>param.maxPolylineLength) del=true;
		if(del)
		{
		  delete pline;
			polylines.RemoveAt(pos0);
		}
		if(pr)
		{
			int count=polylines.GetCount();
			if(!pr->Position(count0-count)) break;
			count0++;
		}
	}
}

int Vectorizer::Vectorize(HBITMAP hbitmap)
{
	HDC hdc=CreateCompatibleDC(NULL);
	HGDIOBJ prevBitmap=SelectObject(hdc,hbitmap);

	int res=Vectorize(hdc);

	SelectObject(hdc,prevBitmap);
	DeleteDC(hdc);
	hdc=0;

	return res;
}

int Vectorizer::Vectorize(HDC hdc)
{
	batch.Clear();
	RECT rect;
	if(GetClipBox(hdc,&rect))
	{
		Progress pr(5,_T("Vectorization"));
		while(pr.Cont())
		{
			skeleton.Init(rect,hdc,&pr);
			if(!pr.NextPass()) break;
			if(param.maxSpotAreaFilter && param.maxSpotArea>0) skeleton.RemoveSpots();
			skeleton.Skeletonize(&pr);
			if(!pr.NextPass()) break;
			skeleton.Vectorize(batch,&pr);
			if(!pr.NextPass()) break;
			if(param.mergeFactorSq>0.) batch.Merge(&pr);
			if(!pr.NextPass()) break;
			batch.Filter(&pr);
			break;
		}
//		for(x=0;x<rect.right;x++)
//		for(y=0;y<rect.bottom;y++)
//		{if(skeleton.GetAt(x,y))SetPixel(hdc,x,y,RGB(255,0,0));}		
	}

	int dummy;
	batch.NextPolyline(dummy);
	return batch.Count();
}

void Vectorizer::Draw(CDC& dc,CRect& rect,double& zr)
{
	int mm=dc.SetMapMode(MM_TEXT);
	batch.Draw(dc,rect,zr);
//	dc.SetMapMode(mm);
}

void Batch::SaveSvg(CArchive& ar)
{
	CString s;
	CRect r;
	r.SetRectEmpty();
	GetBoundRect(r);
	s.Format("<svg width=\"%i\" height=\"%i\">\r\n",r.right,r.bottom);
	ar.WriteString(s);
	POSITION pos=polylines.GetHeadPosition(); 
	while(pos)
	{
	  PolyLine* pline =(PolyLine*)polylines.GetNext(pos);
		pline->SaveSvg(ar);
	}
	ar.WriteString("</svg>");
}

inline void updateBound(const CRect& rect, CRect& bound)
{
	if(rect.left<bound.left) bound.left=rect.left;
	if(rect.top<bound.top) bound.top=rect.top;
	if(rect.right>bound.right) bound.right=rect.right;
	if(rect.bottom>bound.bottom) bound.bottom=rect.bottom;
}

bool Batch::GetBoundRect(CRect& r)
{
	bool res=false;
	CRect pr;
	POSITION pos=polylines.GetHeadPosition(); 
	while(pos)
	{
	  PolyLine* pline =(PolyLine*)polylines.GetNext(pos);
		if(pline->GetBoundRect(pr))
		{
			if(!res){r=pr;res=true;}
			else updateBound(pr,r);
		}
	}
	return res;
}

void Batch::SaveVml(CArchive& ar)
{
	CString s;
	CRect r;
	r.SetRectEmpty();
	GetBoundRect(r);
	s.Format("<v:shape style=\"position: absolute; width: %ipx; height: %ipx;\"  coordsize=\"%i,%i\"",r.right,r.bottom,r.right,r.bottom);
	POSITION pos=polylines.GetHeadPosition(); 
	while(pos)
	{
	  ar.WriteString(s);
	  PolyLine* pline =(PolyLine*)polylines.GetNext(pos);
		pline->SaveVml(ar);
	  ar.WriteString("\r\n");
	}
}

void Batch::SaveDxf(CArchive& ar)
{
	ar.WriteString("0\nSECTION\n2\nENTITIES\n0\n");
	POSITION pos=polylines.GetHeadPosition();
	while(pos)
	{
	  PolyLine* pline =(PolyLine*)polylines.GetNext(pos);
		pline->SaveDxf(ar);
	}
	ar.WriteString("ENDSEC\n0\nEOF");
}

void Batch::SaveHtml(CArchive& ar)
{
	ar.WriteString("<html xmlns:v=\"urn:schemas-microsoft-com:vml\">\n");
	ar.WriteString("<head>\n");
	ar.WriteString("<object id=\"VMLRender\" classid=\"CLSID:10072CEC-8CC1-11D1-986E-00A0C955B42E\">\n");
	ar.WriteString("</object>\n");
	ar.WriteString("<style>v\\:*{behavior: url(#VMLRender);}</style>\n");
	ar.WriteString("</head>\n");
	ar.WriteString("<body>\n");
	SaveVml(ar);
	ar.WriteString("</body>\n");
	ar.WriteString("</html>");
}

bool Batch::IsDemo()const
{
	return demo;
}