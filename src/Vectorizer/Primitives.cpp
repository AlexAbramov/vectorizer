// Primitives.cpp: implementation of the Primitives class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Batch.h"
#include "math.h"
#include "Param.h"

void PolyLine::Add(Point& p)
{
	direction ? points.AddTail(p) : points.AddHead(p);
}

bool PolyLine::FromBitMatrix(BitMatrix &image,Point& p)
{
	Clear();
	if(!image.GetAt(p)) return false;
	Point p0=p;
	direction=true;
	if(!image.Start(p)) return false;
	clr=image.CurColor();
	ASSERT(image.GetAt(p));
	Add(p0);
	Add(p);
	while(image.Step(p))
	{
		ASSERT(image.GetAt(p));
		Add(p);
	}
	direction=false;
	p=p0;
	while(image.Step(p))
	{
		ASSERT(image.GetAt(p));
		Add(p);
	}
	param.ReducAlg() ? Reduce() : Reduce1();
	return true;
}

bool PolyLine::IsSegment(POSITION pos0,POSITION pos1)
{
	ASSERT(pos0!=pos1);
	Point& p0=points.GetNext(pos0);
	ASSERT(pos0!=pos1);
	Point& p1=points.GetAt(pos1);
	int x=p1.x-p0.x;
	int y=p1.y-p0.y;
	bool negx=x<0;
	bool negy=y<0;
	if(negx) x=-x;
	if(negy) y=-y;
	double ss=(x*x+y*y)*param.maxDevSq;
	while(pos0!=pos1)
	{
		ASSERT(pos0);
		Point& p=points.GetNext(pos0);
		int dx=p.x-p0.x;
		int dy=p.y-p0.y;
		if(negx) dx=-dx;
		if(negy) dy=-dy;
//		if(dx<0||dy<0) return false;
//		if(dx>x||dy>y) return false;
		double m=x*dy-y*dx;
		if(ss<m*m) return false;
//		if(abs(x*dy-y*dx)>x+y) return false;
	}
	return true;
}

void PolyLine::Reduce()
{
	if(points.GetCount()<=2) return;
	POSITION pos=points.GetHeadPosition();
	POSITION pos0=pos;
	points.GetNext(pos);
	POSITION pos1=pos;
	while(pos)
	{
		points.GetNext(pos);
		if(pos&&IsSegment(pos0,pos)) pos1=pos;
		else
		{
			ASSERT(pos0!=pos1);
		  points.GetNext(pos0);
		  while(pos0!=pos1)
			{
			  POSITION prevPos=pos0;
			  points.GetNext(pos0);
			  points.RemoveAt(prevPos);
			}
			pos1=pos;
		}
	}
}

void PolyLine::Draw(CDC& dc,double& zr)
{
	POSITION pos=points.GetHeadPosition();
	bool first=true;
	while(pos)
	{
	  Point&p =points.GetNext(pos);
		if(first){dc.MoveTo(p.x*zr,p.y*zr);first=false;}
		else dc.LineTo(p.x*zr,p.y*zr);
	}
}

bool PolyLine::CanMerge(bool head,Point& p)
{
	POSITION pos= head ? points.GetHeadPosition() : points.GetTailPosition();
	Point& p1= head ? points.GetNext(pos) : points.GetPrev(pos);
	Point& p0= head ? points.GetNext(pos) : points.GetPrev(pos);
	int x=p1.x-p0.x;
	int y=p1.y-p0.y;
	bool negx=x<0;
	bool negy=y<0;
	if(negx) x=-x;
	if(negy) y=-y;
	int dx=p.x-p0.x;
	int dy=p.y-p0.y;
	if(negx) dx=-dx;
	if(negy) dy=-dy;
	if(dx<x||dy<y) return false;
	double ss=(x*x+y*y)*param.maxGapDevSq;
	double m=x*dy-y*dx;
	return m*m<ss;
}

bool PolyLine::CanMerge(bool head,PolyLine& pline,bool head1)
{
	if(param.colorDetection)
	{
		if(clr!=pline.GetColor()) return false;
	}
	POSITION pos= head ? GetHeadPosition() : GetTailPosition();
	Point& p0= head ? GetNext(pos) : GetPrev(pos);
	Point& p1= head ? GetNext(pos) : GetPrev(pos);
	pos= head1 ? pline.GetHeadPosition() : pline.GetTailPosition();
	Point& p2= head1 ? pline.GetNext(pos) : pline.GetPrev(pos);
	Point& p3= head1 ? pline.GetNext(pos) : pline.GetPrev(pos);	
	if(!CanMerge(head,p2)) return false;
	if(!CanMerge(head,p3)) return false;
	UINT gapSq=p0.DistanceSq(p2);
	if(gapSq>p0.DistanceSq(p3)) return false;
	if(gapSq>p0.DistanceSq(p1)*param.mergeFactorSq) return false;
	if(gapSq>p2.DistanceSq(p3)*param.mergeFactorSq) return false;
	return true;
}

void PolyLine::Merge(bool head, PolyLine &pline, bool head1)
{
	POSITION pos= head1 ? pline.GetHeadPosition() : pline.GetTailPosition();
	while(pos)
	{
		Point& p= head1 ? pline.GetNext(pos) : pline.GetPrev(pos);
		head ? points.AddHead(p) : points.AddTail(p);
	}
}

double PolyLine::Length()
{
	double length=0.0;
	POSITION pos=GetHeadPosition();
	Point* pprev=0;
	while(pos)
	{
		Point& p=GetNext(pos);
		if(pprev)
		{
		  UINT dsq=pprev->DistanceSq(p);
			length+=sqrt((double)dsq);
		}
		pprev=&p;
	}
	return length;
}

void PolyLine::Reduce1()
{
	if(points.GetCount()<=2) return;
	POSITION pos0=points.GetHeadPosition();
	POSITION pos=points.GetTailPosition();
	TestSegment(pos0,pos);
}

void PolyLine::TestSegment(POSITION pos0,POSITION pos1)
{
	ASSERT(pos0!=pos1);
	POSITION pos=pos0;
	Point& p0=points.GetNext(pos);
	ASSERT(pos!=pos1);
	Point& p1=points.GetAt(pos1);
	int x=p1.x-p0.x;
	int y=p1.y-p0.y;
	bool negx=x<0;
	bool negy=y<0;
	if(negx) x=-x;
	if(negy) y=-y;
	double ss=(x*x+y*y)*param.maxDevSq;
	double maxdev=0.0;
	POSITION maxpos=0;
	bool flag=false;
	while(pos!=pos1)
	{
		Point& p=points.GetAt(pos);
		int dx=p.x-p0.x;
		int dy=p.y-p0.y;
		if(negx) dx=-dx;
		if(negy) dy=-dy;
		double m=abs(x*dy-y*dx);
		if(maxdev<m)
		{
			maxdev=m;
			maxpos=pos;
		}
		if(ss<m*m)
		{
			flag=true;
		}
		points.GetNext(pos);
	}
	if(!flag)
	{
		pos=pos0;
		points.GetNext(pos);
		while(pos!=pos1)
		{
			ASSERT(pos);
			POSITION posr=pos;
			points.GetNext(pos);
		  points.RemoveAt(posr);
		}
	  return;
	}
	if(maxpos)
	{
		TestSegment(pos0,maxpos);
		TestSegment(maxpos,pos1);
	}
}

// BitMatrix
//////////////

bool BitMatrix::Start(Point& p)
{
	SetColor(p.x,p.y);
	dx=0;
	dy=0;
	sr=srNone;
	int count=0;
	for(int x=-1;x<=1;x++)
	for(int y=-1;y<=1;y++)
	{
		if(x==0&&y==0) continue;
		if(!IsInside(p.x+x,p.y+y)) continue;
		if(!GetAt(p.x+x,p.y+y)) continue;
		if(!CheckColor(p.x,p.y)) continue;
		switch(count)
		{
		case 0:
			dx=x;
			dy=y;
			sr=srOne;
			break;
		case 1:
			if(abs((int)(x-dx))+abs((int)(y-dy))==1)
			{
				sr=srCouple;
				if(dx!=0&&dy!=0){dx=x;dy=y;}
			}
			break;
		default:
			sr=srMany;
			return false;
		}
		count++;
	}
	if(count==0) return false;
	ASSERT(count<=2);
  SetAt(p,false);
  p.x+=dx;
  p.y+=dy;
  return true;
}

void BitMatrix::Step(Point& p,int index)const
{
	ASSERT(0<=index&&index<=7);
	if(dx!=0&&dy!=0) // diag
	{
		switch(index)
		{
		case 0: p.x+=dx;p.y+=dy;return;
		case 1: p.x+=dx;return;
		case 2: p.y+=dy;return;
		case 3: p.x+=dx;p.y-=dy;return;
		case 4: p.x-=dx;p.y+=dy; return;
		case 5: p.x-=dx;return;
		case 6: p.y-=dy;return;
		case 7: p.x-=dx;p.y-=dy;return;
		}
	}
	else
	{
		if(dx!=0)//dx
		{
			ASSERT(dy==0);
			switch(index)
			{
			case 0: p.x+=dx;return;
			case 1: p.x+=dx;p.y++; return;
			case 2: p.x+=dx;p.y--; return;
			case 3: p.y++; return;
			case 4: p.y--; return;
			case 5: p.x-=dx;p.y++;return;
			case 6: p.x-=dx;p.y--;return;
			case 7: p.x-=dx;return;
			}
		}
		else//dy
		{
			switch(index)
			{
			case 0: p.y+=dy;return;
			case 1: p.y+=dy;p.x++; return;
			case 2: p.y+=dy;p.x--; return;
			case 3: p.x++; return;
			case 4: p.x--; return;
			case 5: p.y-=dy;p.x++;return;
			case 6: p.y-=dy;p.x--;return;
			case 7: p.y+=dy;return;
			}
		}
	}
	return;
}

bool BitMatrix::Step(Point& p)
{
	if(sr!=srCross) SetAt(p,false);
	state.Clear();
	Point pt=p;
	int i;
	for(i=0;i<5;i++)
	{
		pt=p;
		Step(pt,i);
		if(IsInside(pt)&&GetAt(pt)&&CheckColor(pt.x,pt.y)) state.SetBit(i);
	}
	switch(state.BitCount())
	{
		case 0:
			sr=srNone;
			return false;
		case 1:
			sr=srOne;
			for(i=0;i<5;i++) if(state[i]) break;
			break;
		case 2:
			if(state[0]&&(state[3]||state[4])){sr=srCross;i=0;break;}
			else
			{
				bool bDiag=dx!=0&&dy!=0;
				if(state[1]&&state[0]){sr=srCouple;i=bDiag?1:0;break;}
				if(state[1]&&state[3]){sr=srCouple;i=bDiag?1:3;break;}
				if(state[2]&&state[0]){sr=srCouple;i=bDiag?2:0;break;}
				if(state[2]&&state[4]){sr=srCouple;i=bDiag?2:4;break;}				
			}
		  sr=srMany;
			return false;			
		case 3:
			if(state[0]&&((state[1]&&state[2])||(state[3]&&state[4])))
			{
				sr=srCross;
				i=0;
			  break;
			}
		default:
		  sr=srMany;
			return false;
	}
	ASSERT(i>=0&&i<=4);
	if(i!=0) pt=p;
	Step(p,i);
	if(i!=0){dx=p.x-pt.x;dy=p.y-pt.y;}
	return true;
}

bool PolyLine::GetBoundRect(RECT& rect)
{
	if(!boundRect.IsRectNull()){rect=boundRect;return true;}
	POSITION pos=points.GetHeadPosition();
	bool nopoints=true;
	while(pos)
	{
	  Point&p =points.GetNext(pos);
		if(nopoints)
		{
			boundRect.SetRect(p,p);
			nopoints=false;
		}
		else
		{
	    if(p.x<boundRect.left){boundRect.left=p.x;}
			else if(p.x>boundRect.right) boundRect.right=p.x;
	    if(p.y<boundRect.top){boundRect.top=p.y;}
			else if(p.y>boundRect.bottom) boundRect.bottom=p.y;
		}
	}
	if(!nopoints) rect=boundRect;
	return !nopoints;
}

void PolyLine::SaveSvg(CArchive& ar)
{
	CString s;
	ar.WriteString("  <path style=\"fill:none\" stroke=\"RGB(0,0,0)\" d=\"");
	POSITION pos=points.GetHeadPosition();
	bool first=true;
	while(pos)
	{
	  Point&p =points.GetNext(pos);
		if(first)
		{
			ar.WriteString("M ");
			first=false;
		}
		else ar.WriteString(" L ");
		s.Format("%i %i",p.x,p.y);
		ar.WriteString(s);
	}
	ar.WriteString("\"/>\r\n");
}

void PolyLine::SaveVml(CArchive& ar)
{
	ar.WriteString(" path=\"");
	CString s;
	POSITION pos=points.GetHeadPosition();
	bool first=true;
	while(pos)
	{
	  Point&p =points.GetNext(pos);
		if(first)
		{
			ar.WriteString("m ");
			first=false;
		}
		else ar.WriteString(" l ");
		s.Format("%i,%i",p.x,p.y);
		ar.WriteString(s);
	}
	ar.WriteString(" nf e\"/>");
}

void PolyLine::SaveDxf(CArchive& ar)
{
	ar.WriteString("POLYLINE\n8\nTrace\n6\nCONTINUOUS\n66\n1\n0\n");
	CString s;
	POSITION pos=points.GetHeadPosition();
	while(pos)
	{
	  Point&p =points.GetNext(pos);
		s.Format("VERTEX\n8\nTrace\n10\n%i\n20\n%i\n0\n",p.x,p.y);
		ar.WriteString(s);
	}
	ar.WriteString("SEQEND\n0\n");
}