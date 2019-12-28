#include "stdafx.h"
#include "param.h"

Param param;

Param::Param()
{
	maxDev=1.5;
	mergeFactor=2.;
	maxGapDev=5.;
	maxGapLength=120.;
	minPolylineLength=10.;
	maxPolylineLength=1000.;
	transformMethod=tmCenterLine;
	maxSpotArea=10;
  colorDetection=false;
	maxSpotAreaFilter=false;
	minPolylineLengthFilter=false;
	maxPolylineLengthFilter=false;
	Update();
}

void Param::Update()
{
	maxDevSq=maxDev*maxDev;
	mergeFactorSq=mergeFactor*mergeFactor;
	maxGapDevSq=maxGapDev*maxGapDev;
	maxGapLengthSq=maxGapLength*maxGapLength;
}