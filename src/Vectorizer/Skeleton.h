#pragma once

#include "..\CommonLib\BitArray.h"
#include "Primitives.h"
#include "..\CommonLib\Progress.h"

class Batch;

class Skeleton
{
	BitMatrix image;
	CList<int,int> pixels;
	int Width(){return image.SizeX();}
	int Height(){return image.SizeY();}
	void Erode();
	void Dilate();
public:
	void RemoveSpots();
	void Init(const RECT& rect,const HDC hdc,Progress* pr);
	void Vectorize(Batch& batch,Progress* pr=0);
	Skeleton();
	void SetAt(int x,int y){image.SetAt(x,y,true);}
	void Skeletonize(Progress* pr=0);
	bool GetAt(int x,int y){return image.GetAt(x,y);}
private:
	bool Build(PolyLine& pline,int x,int y);
	UINT Pass(bool firstPass);
	void DeleteFlagedPoints();
	int NumberOf01Transitions(bool* p);
	int NumberOfNeighbors(bool* p);
};
