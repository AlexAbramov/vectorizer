//Style{BYTE,[int],[int],[int],[bString]}
//Indexer{initialCapacity}
//
//{bCommand,iParent,bType}

//GObject{id,bFlag,[style],[bString]}
//	GPoint{wpoint}
//	  GText{bAngle}
//	GPolyline{size,coords}
//	  GPolygon{}
//	GComposite{wrect,indexer}
//		GType{srange}
//		  GLib{sindex}

#ifndef CONSTANTS_H
#define CONSTANTS_H

enum GFlags
{
	gfVisible,
  gfStyle,
  gfName,
  gfCaption
}

const BYTE marker[]={-1,-2,-3,-4};

enum GCommands
{
  gcLoadObject,
  gcLoadChildren,
  gcTerminate
}

enum GTypes
{
  gtPoint,
  gtPolyline,
  gtPolygon,
  gtText,
  gtComposite,
  gtType,
  gtLib
}

// StyleItems
//const BYTE siForeColor=1<<0;
//const BYTE siBackColor=1<<1;
//const BYTE siTextColor=1<<2;
//const BYTE siImage=1<<3;

#endif
