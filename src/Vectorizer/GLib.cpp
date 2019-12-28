// GLib.cpp: implementation of the GLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TxmConverter.h"
#include "GLib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLib::GLib(FILE* fp): GType(fp)
{
	int i;
	fscanf(fp,"%i",&i);newline(fp);sindex=(BYTE)i;
	style=new Style(RGB(0,0,255),RGB(196,196,196),RGB(0,0,0));
}

GComposite::GComposite(FILE* fp): GObject(fp),rect(INT_MAX,INT_MAX,INT_MIN,INT_MIN)
{
	int i;
	fscanf(fp,"%i",&i);newline(fp);rect.left=i*km;
	fscanf(fp,"%i",&i);newline(fp);rect.top=i*km;
	fscanf(fp,"%i",&i);newline(fp);rect.right=i*km;
	fscanf(fp,"%i",&i);newline(fp);rect.bottom=i*km;
}

GType::GType(FILE *fp): GComposite(fp)
{
	int i;
	fscanf(fp,"%i",&i);newline(fp);smin=(BYTE)i;
	fscanf(fp,"%i",&i);newline(fp);smax=(BYTE)i;
}

GType::GType(const SMType& smType): GComposite(smType)
{
	smin=smType.SMin();
	smax=smType.SMax();
	style=smType.CreateStyle();
}

GObject::GObject(FILE *fp)
{
	Init();
	const int bufSize=256;
	static char buf[bufSize];
	buf[0]=0;
	fscanf(fp,"%s",buf);newline(fp);
	name=allocString(buf);
}

/*void GLib::Write(int fout)
{
	// GLib
	int initial Capacity=0;
	int id=0;
	_writeInt(fout,GIDSuperTop);
	_writeInt(fout,GTLib);
	_writeInt(fout,GIDTop);
	_writeInt(fout,initialCapacity);
	range.write(fout);
	_writeInt(fout,smin);
	_writeInt(fout,smax);
	_writeInt(fout,stopElement);
}*/

Style* GObject::GetStyle(){if(style) return style; if(parent) return parent->GetStyle(); return 0;}

void GObject::Write(int fout)
{
	writeMarker(fout);
	if(parent) writeInt(fout,parent->Id());
	else writeInt(fout,-1);
	writeByte(fout,Type());
	writeInt(fout,id);
	BYTE flag=0;
	if(visible) flag|=gfVisible;
	if(style) flag|=gfStyle;
	if(name) flag|=gfName;
	if(caption) flag|=gfCaption;
	writeByte(fout,flag);
	if(style) style->Write(fout);
	if(name) writeString(fout,name);
	if(caption) writeString(fout,caption);
}

GObject* GObject::Top(){return parent ? parent->Top() : this;}

inline void decreaseRect(CRect& rect,int sindex)
{
	rect.left>>=sindex;
	rect.top>>=sindex;
	rect.right>>=sindex;
	rect.bottom>>=sindex;
}

inline void increaseRect(CRect& rect,int sindex)
{
	rect.left<<=sindex;
	rect.top<<=sindex;
	rect.right<<=sindex;
	rect.bottom<<=sindex;
}

static int squareId()
{
	static int index=-100;
	return index--;
}

void GLib::Add(const TxmObject& txmObject)
{
	if(txmObject.IsType())
	{
		GType* gtype=0;
		const SMType& smType=(const SMType&)txmObject;
		if(!types.Lookup(smType.Id(),gtype))
		{
			int parentId=smType.ParentId();
			GType* parent=0;
			if(parentId==-1) parent=this;
			else
			{
				if(!types.Lookup(parentId,parent))
					printf("Type %s not found at object %i\n",smType.ParentStrId(),smType.Id());
			}
			if(parent)
			{
				GType* gtype=new GType(smType);
				parent->Add(gtype,false);
				types.SetAt(gtype->Id(),gtype);
			}
		}
		else printf("Type %s already present\n",smType.StrId());
	}
	else
	{
		const SMObject& smObject=(const SMObject&)txmObject;
		GObject* gobj=0;
		if(!objects.Lookup(txmObject.Id(),gobj))
		{
			if(!intersects(rect,smObject.Rect())) return;
			switch(smObject.GetKType())
			{
			case kArea:
				if(smObject.NPoints()>2) gobj=new GPolygon(smObject);
				else printf("Wrong npoints at point object %i\n",smObject.Id());
				break;
			case kLine:
				if(smObject.NPoints()>1) gobj=new GPolyline(smObject);
				else printf("Wrong npoints at line object %i\n",smObject.Id());
				break;
			case kPoint:
				if(smObject.NPoints()==1) gobj=new GPoint(smObject);
				else printf("Wrong npoints at area object %i\n",smObject.Id());
				break;
			}
			if(gobj)
			{
				int typeId=smObject.TypeId();
				GType* gtype=0;
				if(types.Lookup(typeId,gtype))
				{
					ASSERT(gtype);
					CRect rect=smObject.Rect();
					decreaseRect(rect,sindex);
					if(rect.Width()>1||rect.Height()>1)	gtype->Add(gobj);// big object
					else
					{
						if(rect.Width()>0||rect.Height()>0)// big square
						{
							rect.right=rect.left+2;
							rect.bottom=rect.top+2;
						}
						else// square
						{
							rect.right=rect.left+1;
							rect.bottom=rect.top+1;
						}
						increaseRect(rect,sindex);
						GComposite* gcomp=gtype->FindComposite(rect);
						if(gcomp==0)
						{
							gcomp=new GComposite(squareId(),rect);
						  gtype->Add(gcomp);
						}
						gcomp->Add(gobj,false);
					}
				}
				else
				{
					printf("Type %s not found\n",idToStr(typeId));
					delete gobj;
					gobj=0;
				}
			}
		}
		else printf("Object %i already present",txmObject.Id());
	}
}