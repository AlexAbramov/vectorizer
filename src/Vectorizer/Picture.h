#pragma once

class Picture
{
public:
	Picture();
	virtual ~Picture();
	static Picture* Create();
public:
  virtual bool Load(LPCTSTR szFile)=0;
	virtual void Unload()=0;
	virtual void GetSize(SIZE& sz)=0;
	virtual void Draw(HDC hdc,RECT& rectDest,RECT* rectSrc)=0;
	virtual bool IsNull()=0;
	virtual HDC GetDC()=0;
	virtual void ReleaseDC(HDC hdc)=0;
	virtual bool Save(LPCTSTR szFile){return false;}
};