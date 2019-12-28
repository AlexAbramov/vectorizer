#pragma once
	
class CommonLib  
{
public:
	CommonLib();
	virtual ~CommonLib();

};

void Message(LPCTSTR s);
inline CString GetExt(CString path){int i=path.ReverseFind('.'); return i>=0 ? path.Right(path.GetLength()-i-1) : _T("");}
inline CString GetFileName(CString path){int i=path.ReverseFind('\\'); return i>=0 ? path.Right(path.GetLength()-i-1) : path;}
inline void RemoveExt(CString& path){int i=path.ReverseFind('.'); if(i>=0) path.Delete(i,path.GetLength()-i);}
