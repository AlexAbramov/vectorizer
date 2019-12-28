// RotatingGlobe.cpp : implementation file
//

#include "stdafx.h"
#include "commonlib.h"
#include "RotatingGlobe.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double pi=3.1415;
const double gradToRad=pi/180.;
const double pi2=pi+pi;

#define TIMER_ID 1

struct SPOINT
{
	short x,y;
};

struct SPoint
{
	short x,y,r;
};

static SPOINT greenland[]={{-45,60},{-70,76},{-30,80},{-25,70}};
static SPOINT northAmerica[]={{-168,66},{-156,72},{-130,70},{-96,68},{-94,72},{-87,67},{-85,70},{-82,67},{-95,59},{-80,51},{-78,63},{-56,53},{-67,50},{-61,45},{-74,41},{-76,35},{-82,32},{-80,26},{-81,25},{-83,27},{-82,28},{-84,31},{-88,31},{-90,29},{-93,30},{-97,28},{-97,23},{-94,19},{-91,19},{-91,21},{-87,22},{-88,16},{-84,15},{-84,11},{-81,9},{-79,10},{-80,7},{-86,12},{-91,13},{-95,17},{-97,16},{-105,20},{-105,22},{-114,32},{-110,23},{-111,22},{-114,26},{-117,34},{-121,35},{-124,41},{-125,48},{-131,54},{-140,60},{-150,60},{-154,59},{-153,57},{-161,55},{-157,58},{-165,60}};
static SPOINT cuba[]={{-74,20},{-77,20},{-81,22},{-85,22},{-82,23},{-80,23}};
static SPOINT southAmerica[]={{-82,-5},{-77,3},{-79,9},{-77,8},{-72,12},{-62,10},{-54,5},{-52,4},{-50,0},{-36,-5},{-35,-7},{-38,-13},{-42,-23},{-48,-26},{-48,-29},{-58,-38},{-63,-39},{-66,-47},{-68,-46},{-68,-50},{-65,-55},{-73,-55},{-75,-50},{-70,-18}};
static SPOINT irland[]={{-10,52},{-10,54},{-6,55},{-6,52}};
static SPOINT england[]={{1,51},{-5,50},{-3,54},{-6,56},{-2,58},{-3,56},{2,53}};
static SPOINT euroazia[]={{48,30},{47,29},{52,25},{56,26},{57,24},{60,23},{55,17},{50,15},{43,13},{42,17},{35,28},{33,31},{36,36},{27,36},{26,41},{23,40},{24,37},{22,37},{14,45},{12,44},{18,40},{16,38},{16,40},{9,45},{0,40},{-5,36},{-10,37},{-10,43},{-2,43},{-1,46},{-5,48},{2,50},{5,53},{9,54},{8,56},{10,57},{11,54},{19,55},{13,58},{12,60},{12,66},{13,55},{11,60},{8,58},{7,62},{28,72},{43,66},{105,77},{130,71},{180,67},{157,52},{157,56},{164,63},{135,55},{140,54},{140,47},{128,40},{129,36},{126,34},{125,40},{120,40},{120,35},{122,30},{115,23},{108,22},{106,19},{109,14},{109,11},{105,13},{101,13},{99,10},{104,5},{105,2},{102,3},{98,8},{98,13},{92,22},{87,22},{80,15},{80,10},{77,8},{73,16},{72,22},{71,21},{66,25},{58,26},{57,27},{55,26},{50,30}};
static SPOINT africa[]={{10,3},{6,5},{4,7},{-2,5},{-8,5},{-18,15},{-17,22},{-10,30},{-6,35},{-3,34},{10,36},{11,33},{20,31},{22,33},{33,31},{40,15},{44,11},{52,12},{38,-6},{42,-15},{35,-20},{27,-33},{20,-34},{12,-18},{13,-10},{9,-1}};
static SPOINT madagascar[]={{51,-15},{47,-25},{44,-26},{43,-22},{44,-20},{43,-17},{46,-15},{48,-13}};
static SPOINT japan[]={{141,36},{131,32},{130,33},{133,36},{136,36},{139,38},{140,41},{142,40}};
static SPOINT japan1[]={{140,42},{142,46},{145,43},{143,42}};
static SPOINT philippines[]={{124,6},{120,14},{122,18},{126,8}};
static SPOINT kalimantan[]={{110,-3},{109,1},{117,6},{119,5},{118,1},{116,-4}};
static SPOINT sumatra[]={{105,-6},{95,5},{97,5},{106,-4}};
static SPOINT iava[]={{114,-8},{106,-8},{107,-7},{114,-8}};
static SPOINT australia[]={{115,-34},{114,-25},{120,-20},{126,-14},{129,-15},{132,-11},{136,-12},{135,-14},{140,-18},{142,-11},{148,-20},{153,-25},{154,-29},{150,-38},{146,-39},{140,-37},{138,-33},{136,-35},{130,-32},{117,-35}};
static SPOINT newZealand[]={{172,-34},{176,-38},{177,-37},{175,-42}};	
static SPOINT newZealand1[]={{172,-41},{174,-42},{169,-46},{167,-45}};
static SPOINT antarctida[]={{0,-69},{40,-69},{55,-66},{75,-70},{100,-65},{140,-65},{-180,-78},{-120,-75},{-80,-74},{-60,-64},{-64,-75},{-35,-77},{0,-73}};

class SPolygon
{
private:
	int x0,x1;
	SPoint* array;
	int npoints;
	int NormalizeAngle(int a)
	{
		if(a>180) a-=360;
		if(a<180) a+=360;
		return a;
	}
public:
	int GetNPoints(){return npoints;}
	SPolygon(SPOINT* ar,int size,int rad)
	{		
		npoints=size/sizeof(SPOINT);
		array=new SPoint[npoints];
	  for(int i=0;i<npoints;i++)
		{
			short x=ar[i].x;
			double k=ar[i].y*gradToRad;
			SPoint& p=array[i];
			p.x=x;
			p.y=(short)(rad*sin(-k));
			p.r=(short)(rad*cos(k));
			if(i==0){x0=x1=x;}
			else
			{
				if(NormalizeAngle(x-x0)<0) x0=x;
				if(NormalizeAngle(x-x1)>0) x1=x;
			}
		}
	}
  ~SPolygon(){if(array) delete array;}
	SPoint& GetPoint(int i){ASSERT(0<=i&&i<npoints);return array[i];}
	bool IsVisible(int angle)
	{
		if(NormalizeAngle(x1-x0)>180) return true;
		return IsVisible(angle,x0)|IsVisible(angle,x1);			
	}
	bool IsVisible(int a,int x)
	{
		if(a<0)
		{
			if(x>a&&x<a+180) return true;
			else return false;
		}
		else
		{
			if(x>a||x<a-180) return true;
			else return false;
		}
	}	
};

#define EXT(ar) ar,sizeof(ar),rad

class SPolygons
{
private:
	CList<SPolygon*,SPolygon*> list;
	void AddPolygon(SPolygon* pol)
	{
		list.AddTail(pol);
	}
	int rad;
public:
	int GetRad(){return rad;}
	SPolygons(int rad)
	{
		this->rad=rad;
		AddPolygon(new SPolygon(EXT(greenland)));
		AddPolygon(new SPolygon(EXT(northAmerica)));
		AddPolygon(new SPolygon(EXT(cuba)));
		AddPolygon(new SPolygon(EXT(southAmerica)));
		AddPolygon(new SPolygon(EXT(irland)));
		AddPolygon(new SPolygon(EXT(england)));
		AddPolygon(new SPolygon(EXT(euroazia)));
		AddPolygon(new SPolygon(EXT(africa)));
		AddPolygon(new SPolygon(EXT(madagascar)));
		AddPolygon(new SPolygon(EXT(japan)));
		AddPolygon(new SPolygon(EXT(japan1)));
		AddPolygon(new SPolygon(EXT(philippines)));
		AddPolygon(new SPolygon(EXT(kalimantan)));
		AddPolygon(new SPolygon(EXT(sumatra)));
		AddPolygon(new SPolygon(EXT(iava)));
//		AddPolygon(new SPolygon(EXT(newGuinea)));
		AddPolygon(new SPolygon(EXT(australia)));
		AddPolygon(new SPolygon(EXT(newZealand)));
		AddPolygon(new SPolygon(EXT(newZealand1)));
//		AddPolygon(new SPolygon(EXT(antarctida)));
	}
	~SPolygons()
	{
    POSITION pos = list.GetHeadPosition();
		while(pos)
		{
			SPolygon* pol=list.GetNext(pos);
			ASSERT(pol);
			delete pol;
		}
	}
	void Paint(CDC& dc,int xc,int yc,int angle)
	{
    POSITION pos = list.GetHeadPosition();
		while(pos)
		{
			SPolygon* pol=list.GetNext(pos);
			ASSERT(pol);
			if(!pol->IsVisible(angle)) continue;
			bool flagStop=false;
			CPoint* prev=0;
			for(int j=0;j<=pol->GetNPoints()&&!flagStop;j++)
			{
				if(j==pol->GetNPoints())
				{
					j=0;
					flagStop=true;
				}
				SPoint& p=pol->GetPoint(j);
				if(pol->IsVisible(angle,p.x))
				{
					int y=yc+p.y;
					int x=xc-(int)(p.r*cos(gradToRad*(p.x-angle)));
					if(prev)
					{
						dc.MoveTo(prev->x,prev->y);
						dc.LineTo(x,y);
						prev->x=x;
						prev->y=y;
					}
					else prev=new CPoint(x,y);
				}
				else
				{
					if(prev) delete prev;
					prev=0;
				}
				if(flagStop) break;
			}
	  	if(prev) delete prev;
			prev=0;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// RotatingGlobe

RotatingGlobe::RotatingGlobe()
{
  angle=-180;
  dAngle=-1;
	polygons=0;
	hasTimer=false;
}

RotatingGlobe::~RotatingGlobe()
{
	if(polygons) delete polygons;
	ASSERT(!hasTimer);
}


BEGIN_MESSAGE_MAP(RotatingGlobe, CStatic)
	//{{AFX_MSG_MAP(RotatingGlobe)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RotatingGlobe message handlers

void RotatingGlobe::Paint() 
{
  if(!polygons) return;

	CRect rect;
	GetClientRect(&rect);

	CClientDC dc(this); // device context for painting

//	CDC memDC;
//	memDC.CreateCompatibleDC(&dc);
	
//	memDC.SelectObject();

	CBrush backBrush(GetSysColor(COLOR_3DFACE));
	dc.FillRect(rect,&backBrush);

//	CBrush brushOcean(RGB(0,0,128));
	dc.SelectObject(backBrush);

	CPen bluePen(PS_SOLID,1,RGB(0,0,255));
	dc.SelectObject(bluePen);

	CPoint& cp=rect.CenterPoint();
	int rad=polygons->GetRad();
  
	dc.Ellipse(cp.x-rad,cp.y-rad,cp.x+rad,cp.y+rad);

	polygons->Paint(dc,cp.x,cp.y,angle);
}

void RotatingGlobe::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	angle+=dAngle;
	if(angle<-180) angle+=360;
	if(angle>=180) angle-=360;
	this->Invalidate();
//	CStatic::OnTimer(nIDEvent);
}

HBRUSH RotatingGlobe::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if(!hasTimer&&this->m_hWnd)
	{
		if(!polygons)
		{
			CRect rect;
			GetClientRect(&rect);
			int rad=min(rect.Width(),rect.Height())*2/5;
			polygons=new SPolygons(rad);
		}
		SetTimer(TIMER_ID,100,0);
		hasTimer=true;
	}
	// TODO: Change any attributes of the DC here
	Paint();
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}



void RotatingGlobe::OnDestroy() 
{
	CStatic::OnDestroy();
	
	// TODO: Add your message handler code here
	if(hasTimer)
	{
	  KillTimer(TIMER_ID);
		hasTimer=false;
	}
}
