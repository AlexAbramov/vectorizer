#pragma once

#include"..\CommonLib\Progress.h"
#include"Primitives.h"
#include"Skeleton.h"
#include"Skeleton.h"

class Batch
{
	CList<PolyLine*,PolyLine*> polylines;
	bool Merge(POSITION pos,bool head);
	POSITION plinePos;
	POINT* buf;
	int bufsize;
public:
	bool demo;
	bool IsDemo()const;
	void Filter(Progress* pr=0);
	void Merge(Progress* pr=0);
	int Count(){return polylines.GetCount();}
	Batch():buf(0),bufsize(0),plinePos(0),demo(false){}
	~Batch(){Clear();}
	void Add(PolyLine* pline);
	void Clear();
  void Draw(CDC& dc,CRect& rect,double& zr);
	POINT* NextPolyline(int& _count);
	void SaveSvg(CArchive& ar);
	void SaveDxf(CArchive& ar);
	void SaveVml(CArchive& ar);
	bool GetBoundRect(CRect& r);
	void SaveHtml(CArchive& ar);
};

class Vectorizer
{
	Skeleton skeleton;
public:
	Batch batch;
	void Draw(CDC& dc,CRect& rect,double& zr);
	Vectorizer(){}
	int Vectorize(HBITMAP bitmap);
	int Vectorize(HDC hdc);
	bool IsDemo();
	bool IsEmpty(){return batch.Count()<=0;}
};
