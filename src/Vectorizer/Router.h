// Router.h: interface for the Router class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ROUTER_H)
#define ROUTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef PRIMITIVES_H
	#include "Primitives.h"
#endif

class Bitmap
{
	CList<COLORREF,COLORREF> colors;
	HDC hdc;
public:
	COLORREF bgColor;
	Bitmap(){hdc=0;bgColor=WHITE;}
	bool Init(HDC _hdc,const Point& p);
	bool HasColor(COLORREF clr);
	bool GetPixel(int i,int j){return HasColor(::GetPixel(hdc,i,j));}
	bool GetPixel(const Point& p){return HasColor(::GetPixel(hdc,p.x,p.y));}
};

class Router  
{
	Bitmap bitmap;
	// stream data
	int maxLineWidth;
	int maxNPolylines;
	int step;
	int bigStep;
	double wGo,wGap,wWidth;
	int minGapSize;
	int nDirections;
	double minAngle;
	int segmentLengthFilter;
	int minNPointsFilter;
	int maxNPointsFilter;
	// temporary variables
	Point curPoint;
	double curAngle;
	// auxiliary methods
	bool Step();
	bool FindInitialDirection();
	bool FindMaximum(double& dAngle,int& rmax,Point& resPoint);
	int Distance(const double& angle,Point& resPoint);
	int FindDirection(Point& resPoint);
	void AddPoint(Point& p);
	double Weight(const double &angle,int length,Point& resPoint);
	bool FindLineCenter(Point& p);
	double LineWidth(const Point &p);
	int NSteps(const Point& pos,int dx,int dy);
	double FindLocalDirection(int step,Point& resPoint,double& resAngle);
	bool WidthTest(const Point& p);
	bool ReadFile(FILE* fp);
public:
	bool Filter();
	COLORREF BGColor(){return bitmap.bgColor;}
	int MaxLineWidth(){return maxLineWidth;}
	int MaxNPolylines(){return maxNPolylines;}
	bool messageEnabled;
	PolyLine polyline;
	Router();
	virtual ~Router();
	bool Vectorize(const HDC hdc,Point& startPoint);
	void Draw(HDC hdc);
};

#endif // !defined(ROUTER_H)