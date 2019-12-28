// DocManager.cpp: implementation of the DocManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vectorizer.h"
#include "PreviewFileDlg.h"
#include "DocManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DocManager::DocManager(): image(0)
{

}

DocManager::~DocManager()
{
  if(image) delete image;
}

static void AppendFileType(CString& filter, CString& catfilter,
                           LPCTSTR desc, LPCTSTR extn)
{
    filter += desc;
    filter += (TCHAR)'\0';
    filter += extn;
    filter += (TCHAR)'\0';
    if (! catfilter.IsEmpty()) catfilter += ';';
    catfilter += extn;
}


IMPLEMENT_DYNAMIC(DocManager, CDocManager)

BOOL DocManager::DoPromptFileName(CString& fileName,
                                     UINT nIDSTitle,
                                     DWORD lFlags,
                                     BOOL bOpenFileDialog,
                                     CDocTemplate* /* pTemplate */ )
{
  PreviewFileDlg dlgFile(bOpenFileDialog);

  CString title;
  VERIFY(title.LoadString(nIDSTitle));
	CString strFilter,strGrfFlt;

//	CSimpleArray<GUID> aguidFileTypes;
//  HRESULT hResult = CImage::GetExporterFilterString( strFilter, aguidFileTypes,0,0,0);
//	VERIFY(SUCCEEDED(hResult));
  dlgFile.m_ofn.Flags |= OFN_ENABLESIZING/*|OFN_OVERWRITEPROMPT*/;
	if(bOpenFileDialog) dlgFile.m_ofn.Flags |= OFN_FILEMUSTEXIST;

	if(bOpenFileDialog)
	{
		AppendFileType(strFilter,strGrfFlt,_T("BITMAP (*.bmp;*.dib;*.rle)"),_T("*.bmp;*.dib;*.rle"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("GIF (*.gif)"),_T("*.gif"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("JPEG (*.jpg;*.jpeg;)"),_T("*.jpg;*.jpeg"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("TIFF (*.tif;*tiff)"),_T("*.tif;*tiff"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("PNG (*.png)"),_T("*.png"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("ICON (*.ico)"),_T("*.ico"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("METAFILE (*.wmf;*.emf)"),_T("*.wmf;*.emf"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("All supported graphics formats"),(LPCTSTR) strGrfFlt);
		dlgFile.m_ofn.nMaxCustFilter++;
	
		dlgFile.m_ofn.nFilterIndex = dlgFile.m_ofn.nMaxCustFilter;
  }
	else
	{
		AppendFileType(strFilter,strGrfFlt,_T("AutoCAD Interchange (*.dxf)"),_T("*.dxf"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("Vector Markup Language (*.vml)"),_T("*.vml"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("Scalable Vector Graphics (*.svg)"),_T("*.svg"));
		dlgFile.m_ofn.nMaxCustFilter++;

		AppendFileType(strFilter,strGrfFlt,_T("HTML with VML (*.htm)"),_T("*.htm"));
		dlgFile.m_ofn.nMaxCustFilter++;
//		AppendFileType(strFilter,strGrfFlt,_T("All supported vector formats"),(LPCTSTR) strGrfFlt);
//		dlgFile.m_ofn.nMaxCustFilter++;
	}

  // append the "*.*" all files filter
  CString allFilter;
  VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
  AppendFileType(strFilter,strGrfFlt,allFilter,_T("*.*"));
  //  dlgFile.m_ofn.nMaxCustFilter++;

  dlgFile.m_ofn.lpstrFilter = strFilter;
  dlgFile.m_ofn.lpstrTitle = title;

	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
  bool bResult = dlgFile.DoModal() == IDOK ? true : false;
	fileName.ReleaseBuffer();

  if(bResult)
	{
		if(image){delete image;image=0;}
		if(dlgFile.imageStatic.path.CompareNoCase(fileName)==0)
		{
			image=dlgFile.imageStatic.image;
			dlgFile.imageStatic.image=0;
		}
		if(!bOpenFileDialog)
		{
			CString ext;
			switch(dlgFile.m_ofn.nFilterIndex)
			{
			case 1: ext=_T("dxf");	break;
			case 2: ext=_T("vml");	break;
			case 3: ext=_T("svg");	break;
			case 4: ext=_T("htm");	break;
			}
			if(!ext.IsEmpty())
			{
				RemoveExt(fileName);
				fileName+=_T(".")+ext;
			}
		}		  
	}

  return bResult;
}
