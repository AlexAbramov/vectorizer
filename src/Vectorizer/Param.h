#pragma once

enum TransformMethod{tmCenterLine,tmInsideContour,tmOutsideContour};

class Param
{
public:
  double maxDev;
	double mergeFactor;
	double maxGapDev;
	double maxGapLength;
	double minPolylineLength;
	double maxPolylineLength;
	int maxSpotArea;
	bool colorDetection;
	bool maxSpotAreaFilter;
	bool minPolylineLengthFilter;
	bool maxPolylineLengthFilter;
	TransformMethod transformMethod;
// calculated
	double maxDevSq;
	double mergeFactorSq;
	double maxGapDevSq;
	double maxGapLengthSq;
public:
	Param();
	void Update();
	bool ReducAlg(){return maxDev<0.0;}
};

extern Param param;