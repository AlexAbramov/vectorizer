#pragma once

class Progress
{
	HWND hwnd;
	HWND hwndPB;
	const int npass;
	int ipass;
	int range;
	int index;
	bool cont;
	void Draw();
public:
	bool Cont(){return cont;}
	Progress(int npass,LPCTSTR caption=0);
	~Progress(){if(hwnd)DestroyWindow(hwnd);}
	void Range(int range){ASSERT(range>=0);this->range=range;index=0;}
	bool Position(int i);
	bool PositionBack(int i){return Position(range-i);}
	bool NextPass(){ASSERT(ipass<npass);Draw();ipass++;if(ipass>=npass) cont=false; return cont;}
};
