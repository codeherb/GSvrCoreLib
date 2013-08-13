#pragma once

class CRegistry
{
public:
	CRegistry(void);
	virtual ~CRegistry(void);
public:
	BOOL Open(HKEY rootKey, LPCTSTR subKey);	//LPCTSTRÀº LP-LongPointer C-Const TSTR- T StringÀ» ³ªÅ¸³¿
	BOOL Close(VOID);

	BOOL CreateKey(HKEY rootKey, LPCTSTR subKey);
	BOOL DeleteKey(HKEY rootKey, LPCTSTR subKey);

	BOOL SetValue(LPCTSTR valueName, LPCTSTR value);
	BOOL SetValue(LPCTSTR valueName, DWORD value);
	BOOL SetValueForMultiSz(LPCTSTR valueName, LPCTSTR value, DWORD byteLength);
	
	BOOL GetValue(LPCTSTR valueName, LPCTSTR value, LPDWORD bufferLength);
	BOOL GetValue(LPCTSTR valueName, LPDWORD value);

	BOOL DeleteValue(LPCTSTR valueName);
private:
	HKEY mRootKey;
	BOOL mIsOpened;
};

