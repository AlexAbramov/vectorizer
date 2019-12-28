// GLib.h: interface for the GLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(GLIB_H)
#define GLIB_H

#ifndef CONSTANTS_H
#include "Constants.h"
#endif

class IObject
{

};

class IGeometry
{
};

class IPaintStyle
{

};

class Style
{
	COLORREF foreColor;//[stream data]
	COLORREF backColor;//[stream data]
	COLORREF textColor;//[stream data]
	CString imageName;//[stream data]
public:
	Style(COLORREF foreColor=NULL_COLOR,COLORREF backColor=NULL_COLOR,COLORREF textColor=NULL_COLOR,LPCTSTR imageName=0):
		foreColor(foreColor),backColor(backColor),textColor(textColor),imageName(imageName)
	{
	}
	COLORREF ForeColor(){return foreColor;}
	COLORREF BackColor(){return backColor;}
	COLORREF TextColor(){return textColor;}
	LPCTSTR ImageName(){return imageName;}
	void Write(int fout)
	{
		BYTE flag=0;
		if(foreColor!=NULL_COLOR) flag|=siForeColor;
		if(backColor!=NULL_COLOR) flag|=siBackColor;
		if(textColor!=NULL_COLOR) flag|=siTextColor;
		if(!imageName.IsEmpty()) flag|=siImage;
		writeByte(fout,flag);
		if(foreColor!=NULL_COLOR) writeColor(fout,foreColor);
		if(backColor!=NULL_COLOR) writeColor(fout,backColor); 
		if(textColor!=NULL_COLOR) writeColor(fout,textColor); 
		if(!imageName.IsEmpty()) writeString(fout,imageName);
	}
};

class GComposite;

inline LPTSTR allocString(LPCTSTR s)
{
	if(!s) return 0;
	int len=strlen(s);
	if(len==0) return 0;
	return strcpy(new TCHAR[len+1],s);
}

class GObject
{
	friend GComposite;
protected:
	GComposite* parent;
	int id;
	bool visible;
	Style* style;
	TCHAR* name;
	TCHAR* caption;
	void Init(){parent=0;id=0;visible=true;style=0;name=0;caption=0;}
protected:
	GObject(int id){Init();this->id=id;}
	GObject(FILE* fp);
	GObject(const TxmObject& txmObject)
	{
		Init();
		id=txmObject.Id();
		name=allocString(txmObject.Name());
		caption=allocString(txmObject.Caption());
	}
	virtual bool UpdateBound(CRect& bound)const=0;
public:
	virtual ~GObject(){if(style) delete style;if(name) delete[] name;if(caption) delete[] caption;}
	int Id(){return id;}
	bool Visible(){return visible;}
	virtual bool IsComposite(){return false;}
	bool IsType(){return false;}
	Style* GetStyle();
	LPCTSTR Name(){return name;}
	LPCTSTR Caption(){return caption;}
	virtual void Write(int fout);
	GObject* Top();
	virtual BYTE Type()=0;
};

class Indexer
{
	CList<GObject*,GObject*> children;
public:
	Indexer(){}
	~Indexer()
	{
		POSITION pos=children.GetHeadPosition();
		while(pos){delete children.GetNext(pos);}
		children.RemoveAll();
	}
	void Add(GObject* gobj){children.AddTail(gobj);}
	int Size(){return children.GetCount();}
	// iterations
	POSITION GetHeadPosition()const{return children.GetHeadPosition();}
	GObject* GetNext(POSITION& pos){return children.GetNext(pos);}
};

class GComposite: public GObject
{
	friend GComposite;
	Indexer indexer;
	void UpdateBound(const GObject* gobj)
	{
		if(gobj->UpdateBound(rect)&&parent&&parent->Type()!=gtLib) parent->UpdateBound(gobj);
	}
protected:
	CRect rect;
	virtual bool UpdateBound(CRect& bound) const {return updateBound(rect,bound);}
public:
	CRect& Rect(){return rect;}
	GComposite(FILE* fp);
	GComposite(const SMType& smType): GObject(smType), rect(INT_MAX,INT_MAX,INT_MIN,INT_MIN)
	{
	}
	GComposite(int id,const CRect& rect): GObject(id), rect(rect)
	{
	}
	virtual bool IsComposite(){return true;}
	void Add(GObject* gobj,bool updateBound=true)
	{
		ASSERT(gobj);
		indexer.Add(gobj);
		gobj->parent=this;
		if(updateBound) UpdateBound(gobj);
	}
	virtual void Write(int fout)
	{
		GObject::Write(fout);
		writeInt(fout,rect.left);
		writeInt(fout,rect.top);
		writeInt(fout,rect.right);
		writeInt(fout,rect.bottom);
		writeInt(fout,indexer.Size());
	}
	GComposite* FindComposite(const CRect& rect)
	{
		POSITION pos=indexer.GetHeadPosition();
		while(pos)
		{ 
			GObject* gobj=indexer.GetNext(pos);
			if(gobj->IsComposite())
			{
				GComposite* gcomp=(GComposite*) gobj;
				if(gcomp->Rect()==rect)	return gcomp;
			}
		}
		return 0;
	}
	void WriteAll(int fout)
	{
		this->Write(fout);
		POSITION pos=indexer.GetHeadPosition();
		while(pos)
		{ 
			GObject* gobj=indexer.GetNext(pos);
			if(gobj->IsComposite()) ((GComposite*)gobj)->WriteAll(fout);
			else gobj->Write(fout);
		}
	}
	virtual BYTE Type(){return gtComposite;}
};

class GPoint: public GObject
{
	CPoint p;//stream data
	BYTE angle;//stream data
protected:
	virtual bool UpdateBound(CRect& bound) const {return updateBound(p,bound);}
public:
	GPoint(const SMObject& smObject): GObject(smObject),angle(0)
	{
		p=smObject.Point();
		angle=smObject.TextAngle();
	}
	virtual void Write(int fout)
	{
		GObject::Write(fout);
		writeInt(fout,p.x);
		writeInt(fout,p.y);
		writeByte(fout,angle);
	}
	virtual BYTE Type(){return gtPoint;}
};

/*class GText: public GPoint
{
	BYTE angle;//stream data
public:
	GText(const SMObject& smObject): GPoint(smObject)
	{
		angle=smObject.TextAngle();
	}
	virtual void Write(int fout)
	{
		GPoint::Write(fout);
		writeByte(fout,angle);
	}
	virtual BYTE Type(){return gtText;}
};*/

class GPolyline: public GObject
{
protected:
	CRect rect;//stream data
	virtual bool UpdateBound(CRect& bound) const {return updateBound(rect,bound);}
	int npoints;//stream data
	POINT* points;//stream data
public:
	GPolyline(const SMObject& smObject): GObject(smObject),rect(INT_MAX,INT_MAX,INT_MIN,INT_MIN)
	{
		npoints=smObject.NPoints();
		points=new POINT[npoints];
		memcpy(points,smObject.Points(),npoints*8);
		for(int i=0;i<npoints;i++){updateBound(points[i],rect);}
	}
	~GPolyline()
	{
		delete [] points;
	}
	CRect& Rect(){return rect;}
	virtual void Write(int fout)
	{
		GObject::Write(fout);
		writeInt(fout,npoints);
		writeIntArray(fout,(int*)points,npoints<<1);
	}
	virtual BYTE Type(){return gtPolyline;}
};

class GPolygon: public GPolyline
{
public:
	GPolygon(const SMObject& smObject): GPolyline(smObject)
	{		
	}
	virtual BYTE Type(){return gtPolygon;}
//	boolean intersects(Rect wrect){return wrect.intersects(this.wrect);}
};

class GType: public GComposite
{
	BYTE smin,smax;
public:
	GType(FILE* fp);
	GType(const SMType& SMType);
	BYTE SMin(){return smin;}
	BYTE SMax(){return smax;}
/*	bool Intersects(const GType& )
	{
		if(max<r.min||min>r.max) return false;
		return true;
	}*/
	bool Contains(BYTE s){return smin<=s&&s<=smax;}
	void Write(int fout)
	{
		GComposite::Write(fout);
		writeByte(fout,smin);
		writeByte(fout,smax);
	}
	virtual BYTE Type(){return gtType;}
	bool IsType(){return true;}
};

class GLib: public GType
{
	BYTE sindex;
	CMap<int,int,GType*,GType*> types; 
	CMap<int,int,GObject*,GObject*> objects; 
public:
	GLib(FILE* fp);
	void Write(int fout)
	{
 		GType::Write(fout);
		writeByte(fout,sindex);
	}
	BYTE SIndex(){return sindex;}
	void Add(const TxmObject& txmObject);
	virtual BYTE Type(){return gtLib;}
};

#endif // !defined(GLIB_H)
