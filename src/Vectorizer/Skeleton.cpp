// Skeleton.cpp: implementation of the Skeleton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\CommonLib\Progress.h"
#include "Skeleton.h"
#include "Batch.h"
#include "Param.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Skeleton::Skeleton()
{

}

void Skeleton::Skeletonize(Progress* pr)
{
	switch(param.transformMethod)
	{
	case tmInsideContour:
		Erode();
		break;
	case tmOutsideContour:
		Dilate();
		break;
	default:
	  pixels.RemoveAll();
	}
  for(int x = 1; x < Width()-1; x++) 
	for(int y = 1; y < Height()-1; y++)
	{
	  if(image.GetAt(x,y)) pixels.AddTail(image.Index(x,y));
	}
	bool firstPass=true;
	UINT c=Pass(firstPass);
	if(pr) pr->Range(c);
  while(c)
	{
		firstPass=!firstPass;
		if(pr&&!pr->PositionBack(c)) break;
		c=Pass(firstPass);
	}
}

UINT Skeleton::Pass(bool firstPass)
{
  // p7 p0 p1
  // p6 --- p2
  // p5 p4 p3
	bool p[8];
	UINT c = 0;
	POSITION pos=pixels.GetHeadPosition();
	while(pos)
	{
	  int& index=pixels.GetNext(pos);
		ASSERT(index>=0);
	  if(image.GetAt(index) == 0) continue;
		int x=index%Width();
		int y=index/Width();
		p[0] = image.GetAt(x,y+1);
	  p[1] = image.GetAt(x+1,y+1);
	  p[2] = image.GetAt(x+1,y);
	  p[3] = image.GetAt(x+1,y-1);
	  p[4] = image.GetAt(x,y-1);
	  p[5] = image.GetAt(x-1,y-1);
	  p[6] = image.GetAt(x-1,y);
	  p[7] = image.GetAt(x-1,y+1);
	  int n = NumberOfNeighbors(p);
	  if ((n < 2) || (n > 6)) continue;
	  if (NumberOf01Transitions(p) != 1) continue;

	  if (firstPass) 
		{ 
	  	if ((p[0] && p[2] && p[4]) )
	  		continue;
	  	if ((p[2] && p[4] && p[6]))
	  		continue;
			index=-index;
	  	c++;
	  }
	  else 
		{
	  	if ((p[0] && p[2] && p[6])) 
	  		continue;
	  	if ((p[0] && p[4] && p[6]))
	  		continue;
			index=-index;
	  	c++; 			
	  }
	}
	//System.out.println("c="+c);
	if(c) DeleteFlagedPoints();
	return c;
}

int Skeleton::NumberOfNeighbors(bool* p)
{
 	int n=0;
 	if (p[0]) n++;
 	if (p[1]) n++;
 	if (p[2]) n++;
 	if (p[3]) n++;
 	if (p[4]) n++;
 	if (p[5]) n++;
 	if (p[6]) n++;
 	if (p[7]) n++;
 	return n;
}

int Skeleton::NumberOf01Transitions(bool* p)
{
 	int n=0;
 	if ((!p[0]) && p[1]) n++; 
 	if ((!p[1]) && p[2]) n++; 
 	if ((!p[2]) && p[3]) n++; 
 	if ((!p[3]) && p[4]) n++; 
 	if ((!p[4]) && p[5]) n++; 
 	if ((!p[5]) && p[6]) n++; 
 	if ((!p[6]) && p[7]) n++; 
 	if ((!p[7]) && p[0]) n++; 
 	return n;
}

void Skeleton::DeleteFlagedPoints()
{
/*  for(int x = 1; x < Width()-1; x++) 
	for(int y = 1; y < Height()-1; y++) 
	  if (flags.GetAt(x,y))
			image.SetAt(x,y,false);*/
	POSITION pos=pixels.GetHeadPosition();
	while(pos)
	{
		POSITION prevPos=pos;
	  int index=pixels.GetNext(pos);
		if(index<0)
		{
		  image.SetAt(-index,false);
			pixels.RemoveAt(prevPos);
		}
	}
}

void Skeleton::Vectorize(Batch &batch,Progress* pr)
{
	POSITION pos=pixels.GetHeadPosition();
	int count=pixels.GetCount();
	if(pr) pr->Range(count);
	PolyLine* pline=0;
	Point p;
	while(pos)
	{
		if(pr&&!pr->PositionBack(count--)) break;
	  int index=pixels.GetNext(pos);
		ASSERT(index>=0);
		if(!image.GetAt(index)) continue;
		p.x=index%Width();
		p.y=index/Width();
		if(!pline) pline=new PolyLine();
		if(pline->FromBitMatrix(image,p))
		{
			batch.Add(pline);
			pline=0;
		}
	}
	if(pline) delete pline;
	pixels.RemoveAll();
}

void Skeleton::Init(const RECT &rect, const HDC hdc, Progress *pr)
{
	image.Init(rect.right,rect.bottom,hdc,param.colorDetection);
	const COLORREF bgColor=WHITE;
	int x,y;
	if(pr) pr->Range(rect.right);
	for(x=0;x<rect.right;x++)
	{
		for(y=0;y<rect.bottom;y++)
		{
			COLORREF clr=GetPixel(hdc,x,y);
			if(clr!=bgColor&&clr!=CLR_INVALID) SetAt(x,y);
		}
		if(pr&&!pr->Position(x)) break;
	}
}

void Skeleton::RemoveSpots()
{
	CArray<Point,Point&> spot;
  for(int x = 1; x < Width()-1; x++) 
	for(int y = 1; y < Height()-1; y++) 
	{
		if(GetAt(x-1,y)&&GetAt(x,y-1)&&!GetAt(x,y))
		{
			spot.RemoveAll();
			Point p(x,y);
			spot.Add(p);
			int shift=0;
			int index=0;
			while(index<spot.GetSize()&&spot.GetSize()<=param.maxSpotArea)
			{
				p=spot[index];
				switch(shift)
				{
				case 0:
					p.x--;
					break;
				case 1:
					p.y--;
					break;
				case 2:
					p.x++;
					break;
				case 3:
					p.y++;
					break;
				}
				if(image.IsInside(p)&&!image.GetAt(p))
				{
					int i;
					for(i=0;i<spot.GetSize();i++){ if(spot[i]==p) break;}
					if(i==spot.GetSize()) spot.Add(p);
				}
				if(shift<3) shift++;
				else{shift=0;index++;}
			}
			if(spot.GetSize()<=param.maxSpotArea)
			{
				for(int i=0;i<spot.GetSize();i++){image.SetAt(spot[i],true);}				
			}
		}
	}
}

void Skeleton::Erode()
{
	pixels.RemoveAll();
  for(int x = 1; x < Width()-1; x++) 
	for(int y = 1; y < Height()-1; y++)
	{
	  if(image.GetAt(x,y)&&image.GetAt(x-1,y)&&image.GetAt(x+1,y)&&image.GetAt(x,y-1)&&image.GetAt(x,y+1))
  	  pixels.AddTail(image.Index(x,y));
	}
	POSITION pos=pixels.GetHeadPosition();
	while(pos) image.SetAt(pixels.GetNext(pos),false);
	pixels.RemoveAll();
}

void Skeleton::Dilate()
{
	pixels.RemoveAll();
  for(int x = 1; x < Width()-1; x++) 
	for(int y = 1; y < Height()-1; y++)
	{
	  if(!image.GetAt(x,y))
		{
			if(image.GetAt(x-1,y)||image.GetAt(x+1,y)||image.GetAt(x,y-1)||image.GetAt(x,y+1))
  	    pixels.AddTail(image.Index(x,y));
		}
	}
	image.Clear();
	POSITION pos=pixels.GetHeadPosition();
	while(pos)
	{
	  int index=pixels.GetNext(pos);
	  image.SetAt(index,true);
	}
	pixels.RemoveAll();
}