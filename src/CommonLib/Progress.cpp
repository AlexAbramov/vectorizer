// Progress.cpp: implementation of the Progress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Progress.h"
#include "clresource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM  lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: 
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDCLOSE:     
		case IDCANCEL:
			PostQuitMessage(0);
			return TRUE;
		}
	default:
		return FALSE;
	};
}

Progress::Progress(int npass,LPCTSTR caption):npass(npass)
{
	hwnd=CreateDialog(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDD_PROGRESS),GetActiveWindow(),DlgProc);
	if(caption) SetWindowText(hwnd,caption);
	hwndPB = GetDlgItem(hwnd,IDC_PROGRESS);
	ASSERT(npass>0);
	SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessage(hwndPB, PBM_SETSTEP, (WPARAM) 1, 0);
	ipass=0;
	range=0;
	index=0;
	cont=true;
}

void Progress::Draw()
{
	MSG msg;
	while(PeekMessage(&msg, 0, 0, 0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg, 0, 0, 0))
		{
			cont=false;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(cont) SendMessage(hwndPB, PBM_SETPOS, (WORD)index, 0);
}

bool Progress::Position(int i)
{
	if(range>0)
	{
		int _index=(ipass*(double)range+i)*(100./npass)/range;
		if(_index>100) _index=100;
		if(_index>index)
		{
			index=_index;
			Draw();
		}
	}
	return cont;
}
