#pragma once

const double M_PI=3.14159;
const double M_PI2=M_PI+M_PI;
const COLORREF WHITE=RGB(255,255,255);

#include "..\CommonLib\BitArray.h"

class Point: public POINT
{
public:
	Point(){x=0;y=0;}
	Point(int x,int y){this->x=x;this->y=y;}
	Point(const Point& p){x=p.x;y=p.y;}
	operator DWORD()const{return (DWORD)(x+y);}
	Point& operator <<=(BYTE n){x<<=n;y<<=n;return *this;}
	Point& operator >>=(BYTE n){x>>=n;y>>=n;return *this;}
	Point operator <<(BYTE n)const{return Point(x<<n,y<<n);}
	Point operator >>(BYTE n)const{return Point(x>>n,y>>n);}
	Point operator -(int i)const{return Point(x-i,y-i);}
	Point operator +(int i)const{return Point(x+i,y+i);}
	UINT DistanceSq(const Point& p)const{return (x-p.x)*(x-p.x)+(y-p.y)*(y-p.y);}
	bool operator == (Point& p){return x==p.x&&y==p.y;}
};

enum StepResult{srNone,srOne,srCouple,srCross,srMany};

class BitMatrix
{
	int sizex;
	int sizey;
	BitArray ar;
	//iter vars
	StepResult sr;
	int dx,dy;
	BitArray state;
	void Step(Point& p,int index)const;
	// color detection
	HDC hdc;
	COLORREF clr;
	bool colorDetection;
	bool CheckColor(int x,int y)const{return colorDetection ? GetPixel(hdc,x,y)==clr : true;}
public:
	void SetColor(int x,int y){this->clr=GetPixel(hdc,x,y);}
	COLORREF CurColor(){return colorDetection ? clr : 0;}
	StepResult GetStepResult(){return sr;}
	int SizeX(){return sizex;}
	int SizeY(){return sizey;}
	int Size(){return sizex*sizey;}
	BitMatrix(){sizex=0;sizey=0;state.BitSize(5);hdc=0;colorDetection=false;clr=0;}
	void Init(int sizex,int sizey,HDC hdc,bool colorDetection)
	{
		this->hdc=hdc;
		this->colorDetection=colorDetection;
		clr=0;
		if(this->sizex!=sizex || this->sizey!=sizey)
		{
		  ar.BitSize(sizex*sizey);
		  this->sizex=sizex;
		  this->sizey=sizey;
		}
		Clear();
	}
	bool GetAt(int x,int y)const{return ar.GetBit(sizex*y+x);}
	bool GetAt(Point p)const{return ar.GetBit(sizex*p.y+p.x);}
	bool GetAt(int index)const{return ar.GetBit(index);}
	void SetAt(int x,int y,bool v){ar.SetBit(sizex*y+x,v);}
	void SetAt(Point p,bool v){ar.SetBit(sizex*p.y+p.x,v);}
	void SetAt(int index,bool v){ar.SetBit(index,v);}
	void Clear(){ar.Clear();}
	bool Start(Point& p);
	bool Step(Point& p);
	bool IsInside(const Point& p)const{return IsInside(p.x,p.y);}
	bool IsInside(int x,int y)const{return x>=0&&x<sizex&&y>=0&&y<sizey;}
	int Index(int x,int y){return y*sizex+x;}
};

class PolyLine
{
	bool direction;
	bool IsSegment(POSITION pos0,POSITION pos1);
	void Reduce();
	CList<Point,Point&> points;
	void TestSegment(POSITION pos0,POSITION pos1);
	COLORREF clr;
public:
	COLORREF GetColor(){return clr;}
	CRect boundRect;
	int NPoints(){return points.GetCount();}
	double Length();
	void Merge(bool head,PolyLine& pline,bool head1);
	bool visible;
	Point& Head(){return points.GetHead();}
	Point& Tail(){return points.GetTail();}
	Point& HeadOrTail(bool head){if(head) return Head(); return Tail();}
	POSITION GetHeadPosition()const{return points.GetHeadPosition();}
	POSITION GetTailPosition()const{return points.GetTailPosition();}
	Point& GetNext(POSITION& pos){return points.GetNext(pos);}
	Point& GetPrev(POSITION& pos){return points.GetPrev(pos);}
	bool FromBitMatrix(BitMatrix& image, Point& p);
	void Draw(CDC& dc,double& zr);
	PolyLine(){Clear();}
	~PolyLine(){}
	void Clear(){points.RemoveAll();direction=true;visible=true;boundRect.SetRectEmpty();}
	int Count(){return points.GetCount();}
	void Add(Point& p);
	void Direction(bool direction){this->direction=direction;}
	bool CanMerge(bool head,Point& p);
	bool CanMerge(bool head,PolyLine& pline,bool head1);
  bool GetBoundRect(RECT& rect);
  void SaveSvg(CArchive& ar);
  void SaveVml(CArchive& ar);
  void SaveDxf(CArchive& ar);
private:
	void Reduce1();
};
