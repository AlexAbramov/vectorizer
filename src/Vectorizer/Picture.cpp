#include "stdafx.h"
#include "Vectorizer.h"
#include "Picture.h"

class AtlImage: public Picture
{
  CImage image;
public:
	~AtlImage(){Unload();}
	virtual bool Load(LPCTSTR szFile){if(!IsNull())Unload(); return SUCCEEDED(image.Load(szFile));}
	virtual void Unload(){image.Destroy();}
	virtual void GetSize(SIZE& sz){if(IsNull()){sz.cx=0;sz.cy=0;}else{sz.cx=image.GetWidth();sz.cy=image.GetHeight();}}
	virtual void Draw(HDC hdc,RECT& rectDest,RECT* rectSrc){rectSrc ? image.Draw(hdc,rectDest,*rectSrc) : image.Draw(hdc,rectDest);}
	virtual bool IsNull(){return image.IsNull();}
	virtual HDC GetDC(){return image.GetDC();}
	virtual void ReleaseDC(HDC hdc){image.ReleaseDC();};
	virtual bool Save(LPCTSTR szFile){return SUCCEEDED(image.Save(szFile));}
};

#ifdef GDIPLUS

using namespace Gdiplus;

static void InitRect(Rect& rect,const RECT& r)
{
	rect.X=r.left;
	rect.Y=r.top;
	rect.Width=r.right-r.left;
	rect.Height=r.bottom-r.top;
}

class ImagePlus: public Picture
{
  Image* image;
	Graphics* gr;
public:
	ImagePlus():image(0),gr(0){}
	~ImagePlus(){Unload();}
	virtual bool Load(LPCTSTR szFile)
	{
		Unload(); 
		int l=strlen(szFile);
		WCHAR* s=new WCHAR[l+1];
		for(int i=0;i<=l;i++) s[i]=szFile[i];
		image=Image::FromFile(s);
		delete []s;
		s=0;
		return image!=0;
	}
	virtual void Unload()
	{
		if(gr){delete gr; gr=0;}
		if(image){delete image;image=0;}
	}
	virtual void GetSize(SIZE& sz){if(!image){sz.cx=10;sz.cy=10;}else{sz.cx=image->GetWidth();sz.cy=image->GetHeight();}}
	virtual void Draw(HDC hdc,RECT& rectDest,RECT* rectSrc)
	{
		if(!image) return; 
		Graphics g(hdc);
		Rect rd; InitRect(rd,rectDest);
		if(rectSrc)
		{
		  CRect rs(*rectSrc);
			g.DrawImage(image,rd,rs.left,rs.top,rs.Width(),rs.Height(),UnitPixel);
		}
		else g.DrawImage(image,rd,0,0,image->GetWidth(),image->GetHeight(),UnitPixel);
	}
	virtual bool IsNull(){return image==0;}
	virtual HDC GetDC()
	{
		if(!image) return 0;
		if(!gr){gr=new Graphics(image);}
		return gr ? gr->GetHDC() : 0;
	}
	virtual void ReleaseDC(HDC hdc){if(gr) gr->ReleaseHDC(hdc);}
};

#endif

Picture::Picture()
{
}

Picture::~Picture()
{
//  Unload();
}

Picture* Picture::Create()
{
	return new AtlImage();
//	return new ImagePlus();
}
