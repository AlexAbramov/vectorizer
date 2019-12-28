// Router.cpp: implementation of the Router class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommonLib.h"
#include "Router.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

Param param;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// NEWLINE
static int newline(FILE* fp) { int k=0; while(1){ k=fgetc(fp);if(k==10||k==-1) break;} return k; }

bool Router::ReadFile(FILE* fp)
{
  #define CHECK(res) if(res<=0||res==EOF) return false;
	int res;
  res=fscanf(fp,"%i",&maxLineWidth);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&maxNPolylines);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&minGapSize);CHECK(res);newline(fp);
  res=fscanf(fp,"%lf",&minAngle);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&step);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&bigStep);CHECK(res);newline(fp);
  res=fscanf(fp,"%lf",&wGo);CHECK(res);newline(fp);
  res=fscanf(fp,"%lf",&wGap);CHECK(res);newline(fp);
  res=fscanf(fp,"%lf",&wWidth);CHECK(res);newline(fp);
  res=fscanf(fp,"%X",&bitmap.bgColor);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&nDirections);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&segmentLengthFilter);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&minNPointsFilter);CHECK(res);newline(fp);
  res=fscanf(fp,"%i",&maxNPointsFilter);CHECK(res);newline(fp);
	return true;
}

Router::Router()
{
	messageEnabled=false;
  FILE * fp=fopen("router.dat","r");
	bool read=false;
  if(fp)
  {
		read=ReadFile(fp);
    fclose(fp);
  }
  if(!read)
  {
		maxLineWidth=20;
		maxNPolylines=1000;
		step=20;
		bigStep=120;
		wGo=.8;
		wGap=.4;
		wWidth=.7;
		minGapSize=3;
		bitmap.bgColor=WHITE;
		nDirections=36;
		minAngle=.3;
		segmentLengthFilter=20;
		minNPointsFilter=2;
		maxNPointsFilter=40;

    fp=fopen("router.dat","w");
    fprintf(fp,"%i\t- maxLineWidth\n",maxLineWidth);
    fprintf(fp,"%i\t- maxNPolylines\n",maxNPolylines);
    fprintf(fp,"%i\t- minGapSize\n",minGapSize);
    fprintf(fp,"%.2f\t- minAngle\n",minAngle);
    fprintf(fp,"%i\t- step\n",step);
    fprintf(fp,"%i\t- bigStep\n",bigStep);
    fprintf(fp,"%.2f\t- wGo\n",wGo);
    fprintf(fp,"%.2f\t- wGap\n",wGap);
    fprintf(fp,"%.2f\t- wWidth\n",wWidth);
    fprintf(fp,"%X\t- bgColor\n",bitmap.bgColor);
    fprintf(fp,"%i\t- nDirections\n",nDirections);
    fprintf(fp,"%i\t- segmentLengthFilter\n",segmentLengthFilter);
    fprintf(fp,"%i\t- minNPointsFilter\n",minNPointsFilter);
    fprintf(fp,"%i\t- maxNPointsFilter\n",maxNPointsFilter);
    fclose(fp);
  }
}

Router::~Router()
{
}

bool Router::Vectorize(const HDC hdc,Point& startPoint)
{
	polyline.Clear();

	if(!bitmap.Init(hdc,startPoint)) 
	{
		if(messageEnabled) Message(_T("Background here"));
		return false;
	}

	if(!FindLineCenter(startPoint))
	{
//		if(messageEnabled) Message(_T("The object isn't recognized as line"));
//		return false;
	}

	AddPoint(startPoint);
	
	if(!FindInitialDirection())
	{
		if(messageEnabled) Message(_T("Try to click another point"));
		return false;
	}
	double startAngle=curAngle;
	while(Step()){if(polyline.NPoints()>2) polyline.DrawSegment(hdc);}
//	polyline.DrawSegment(hdc,false);
	polyline.SetDirection(false);
	curPoint=startPoint;
	if(startAngle>M_PI) curAngle=startAngle-M_PI;
	else curAngle=startAngle+M_PI;
	while(Step()){polyline.DrawSegment(hdc);}
	polyline.DrawSegment(hdc,false);
	return polyline.NPoints()>=2;
}

bool Router::Step()
{	
	if(polyline.NPoints()>maxNPointsFilter) return false;
	Point resPoint;
/*	int d=Distance(curAngle,resPoint);
	if(d>0)
	{
		if(d+d>maxLineWidth) FindLineCenter(resPoint);
		AddPoint(resPoint);
		return true;
	}*/
	
	double a0=curAngle;
	int d=FindDirection(resPoint);
	if(d>step)
	{
//		FindLineCenter(resPoint);
		AddPoint(resPoint);
		return true;
	}
	curAngle=a0;

	double resAngle;

	double w=FindLocalDirection(bigStep,resPoint,resAngle);
	if(w>wGap)
	{
		curAngle=resAngle;
		AddPoint(resPoint);
		return true;		
	}
	return false;
}

int Router::Distance(const double& angle,Point& resPoint)
{
	double cosa=cos(angle);
	double sina=sin(angle);
	int gapCount=0;
	resPoint=curPoint;
	for(int r=1;;r++)
	{
		int x=curPoint.x+r*cosa;
		int y=curPoint.y+r*sina;
		if(bitmap.GetPixel(x,y))
		{
			resPoint.x=x;
			resPoint.y=y;
			gapCount=0;
		}
		else
		{
			gapCount++;
			if(gapCount>=minGapSize)	
				break;
		}
	}
	if(resPoint.x!=curPoint.x||resPoint.y!=curPoint.y) return r-gapCount;
	else return 0;
}

bool Router::FindInitialDirection()
{	
	int rmax=0;
	Point p,resPoint;
	double dAngle=M_PI2/nDirections;
	for(int i=0;i<nDirections;i++)
	{
		double a=i*dAngle;
		int r=Distance(a,p);
		if(r>rmax)
		{
			rmax=r;
			resPoint=p;
			curAngle=a;
		}
	}
	if(rmax>maxLineWidth)
	{
		while(FindMaximum(dAngle,rmax,resPoint));
//		FindLineCenter(resPoint);
		AddPoint(resPoint);
		return true;
	}
	return false;
}

bool Router::FindMaximum(double &dAngle,int& rmax,Point& resPoint)
{
	dAngle*=.5;
	Point p;
	int r=Distance(curAngle-dAngle,p);
	if(r>rmax)
	{
		rmax=r;
		resPoint=p;
		curAngle-=dAngle;
		return true;
	}
	int rr=Distance(curAngle+dAngle,p);
	if(rr>rmax)
	{
		rmax=rr;
		resPoint=p;
		curAngle+=dAngle;
		return true;
	}
	if(r==rmax&&rr==rmax)
	{
		return false;
	}
	else return true;
}

int Router::FindDirection(Point& resPoint)
{	
	int rmax=0;
	double amax=0.;
	Point p;
	double dAngle=M_PI2/nDirections;
	int imax;
	int ii=(M_PI-minAngle)/dAngle;
	for(int i=-ii;i<=ii;i++)
	{
		double a=curAngle+i*dAngle;
		int r=Distance(a,p);
		if(r>rmax&&WidthTest(p))
		{
			rmax=r;
			resPoint=p;
			amax=a;
			imax=i;
		}
	}
	if(abs(imax)==ii) return 0;
	if(rmax>maxLineWidth)
	{
		curAngle=amax;
		while(FindMaximum(dAngle,rmax,resPoint));
		return rmax;
	}
	return 0;
}

void Router::AddPoint(Point &p)
{
	if(wWidth>0.)
	{
		double lineWidth=LineWidth(p);
		polyline.AddPoint(p,&lineWidth);
	}
	else polyline.AddPoint(p);
	curPoint=p;
}

double Router::Weight(const double &angle,int length,Point& resPoint)
{
	int count=0;
	double cosa=cos(angle);
	double sina=sin(angle);
	resPoint=curPoint;
	int localCount=0,maxCount=0,gapCount=0;
	for(int r=1;r<=length;r++)
	{
		int x=curPoint.x+r*cosa;
		int y=curPoint.y+r*sina;
		if(bitmap.GetPixel(x,y))
		{
			if(localCount>maxCount)
			{
				resPoint.x=x;resPoint.y=y;maxCount=localCount;
			}
			count++;
			localCount++;
			gapCount=0;
		}
		else gapCount++;
		if(gapCount>=minGapSize) localCount=0;
	}
	return double(count)/length;
}

/*
bool Router::FindDirection(Point &resPoint)
{	
	double angleMax=-1.;
	double maxWeight=-1.;
	Point bufPoint;
	int nDirections=M_PI2*step;
	for(int i=0;i<nDirections;i++)
	{
		double a=M_PI2*i/nDirections;
		double weight=1.;
		
		if(angle>=0.)
		{
			if(fabs(a-M_PI)<.5) continue;
//			weight=.5*(1.+fabs(cos(a+a)));
			a+=angle;
			if(a>=M_PI2) a-=M_PI2;
		}
		
		weight*=Weight(curPoint,a,bufPoint,step);
		
		if(maxWeight<weight)
		{
			maxWeight=weight;
			angleMax=a;
			resPoint=bufPoint;
		}
	}

	if(maxWeight<p1&&angle>=0.)
	{
		double angleMax1=-1.;
		double maxWeight1=-1.;
		Point resPoint1;
		nDirections=bigStep*dAngle;
		for(int i=-nDirections;i<=nDirections;i++)
		{
			double a=i*dAngle/nDirections+angle;
			if(a>=M_PI2) a-=M_PI2;
			double weight=Weight(curPoint,a,bufPoint,bigStep);
			if(maxWeight1<weight)
			{
				maxWeight1=weight;
				angleMax1=a;
				resPoint1=bufPoint;
			}
		}
		if(maxWeight1>p2)
		{
			angle=angleMax1;
			resPoint=resPoint1;
			return true;
		}
	}
	
	angle=angleMax;
	return maxWeight>p3;
}
*/

bool Router::FindLineCenter(Point& p)
{
	if(!bitmap.GetPixel(p)) return false;
	int x=NSteps(p,1,0);
	int xx=NSteps(p,-1,0);
	int y=NSteps(p,0,1);
	int yy=NSteps(p,0,-1);
	if(x+xx<y+yy&&x+xx>0)
	{
		if(x+xx<maxLineWidth)
		{
			p.x+=(x-xx)/2;
			return true;
		}
	}
	else
	{
		if(y+yy<maxLineWidth&&y+yy>0)
		{
			p.y+=(y-yy)/2;
			return true;
		}
	}
	x=NSteps(p,1,1);
	xx=NSteps(p,-1,-1);
	y=NSteps(p,1,-1);
	yy=NSteps(p,-1,1);
	if(x+xx<y+yy&&x+xx>0)
	{
		if(x+xx<maxLineWidth)
		{
			p.x+=(x-xx)/2;
			p.y+=(x-xx)/2;
			return true;
		}
	}
	else
	{
		if(y+yy<maxLineWidth&&y+yy>0)
		{
			p.x+=(y-yy)/2;
			p.y-=(y-yy)/2;
			return true;
		}
	}
	return false;
}

int Router::NSteps(const Point& pos,int dx,int dy)
{
	int nsteps=0;
	int x=pos.x;
	int y=pos.y;
	while(true)
	{
		x+=dx;
		y+=dy;
		if(!bitmap.GetPixel(x,y)||nsteps>=maxLineWidth) break;
		nsteps++;
	}
	return nsteps;
}


double Router::LineWidth(const Point &p)
{
	int ss[4];
	ss[0]=1+NSteps(p,1,0)+NSteps(p,-1,0);
	ss[1]=1+NSteps(p,0,1)+NSteps(p,0,-1);
	ss[2]=1+NSteps(p,1,1)+NSteps(p,-1,-1);
	ss[3]=1+NSteps(p,1,-1)+NSteps(p,-1,1);
	int imin=0;
	for(int i=1;i<4;i++) if(ss[i]<ss[imin]){imin=i;}
	int imin2=0;
	if(imin2==imin) imin2=1;
	for(i=imin2+1;i<4;i++) if(i!=imin && ss[i]<ss[imin2]){imin2=i;}
	if(ss[imin]>maxLineWidth) return maxLineWidth;
	if(ss[imin2]>maxLineWidth) return ss[imin];
	return (ss[imin]+ss[imin2])*.5;
}

double Router::FindLocalDirection(int step,Point& resPoint,double& resAngle)
{	
	double wmax=0.;
	Point p;
	int	nDirections=step*minAngle;
	double dAngle=minAngle/nDirections;
	for(int i=-nDirections;i<=nDirections;i++)
	{
		double a=curAngle+i*dAngle;
		double w=Weight(a,step,p);
		if(wmax<w)
		{
			wmax=w;
			resAngle=a;
			resPoint=p;
		}
	}
	return wmax;
}

bool Router::WidthTest(const Point &p)
{
	if(wWidth<=0||polyline.NPoints()==0) return true;
	double lineWidth=LineWidth(p);
	double avgLineWidth=polyline.Width();
	return fabs(lineWidth-avgLineWidth)<avgLineWidth*wWidth;
}

void Router::Draw(HDC hdc)
{
	POSITION pos = polyline.points.GetHeadPosition();
	for(int i=0;i<polyline.points.GetCount();i++)
	{
		Point& p=polyline.points.GetNext(pos);
		if(i==0) ::MoveToEx(hdc,p.x,p.y,0);
		else ::LineTo(hdc,p.x,p.y);
 	}
}

bool Router::Filter()
{
	int npoints=polyline.NPoints();
	if(npoints<minNPointsFilter||npoints>maxNPointsFilter) return false;
	if(polyline.MaxSegmentLength()<segmentLengthFilter) return false;
	return true;
}

bool Bitmap::Init(HDC _hdc, const Point& p)
{
	hdc=_hdc;
	colors.RemoveAll();
	if(::GetPixel(hdc,p.x,p.y)==bgColor) return false;
	int r=3;
	for(int i=-r;i<=r;i++)
	for(int j=-r;j<=r;j++)
	{
		COLORREF clr=::GetPixel(hdc,p.x+i,p.y+j);
		if(clr!=bgColor&&!HasColor(clr))
		{
			colors.AddTail(clr);
		}
	}
	return colors.GetCount()>0;
}

bool Bitmap::HasColor(COLORREF clr)
{
	if(clr==bgColor||clr==CLR_INVALID||colors.IsEmpty()) return false;
	return colors.Find(clr)!=NULL;
}

