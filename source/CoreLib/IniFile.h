#pragma once

class CIniFile
{
public:
	CIniFile(void);
	virtual ~CIniFile(void);

public:
	BOOL Open(LPCTSTR fileName);
	BOOL Close(void);

	BOOL SetValue(LPCTSTR keyName, LPCTSTR valueName, LPCTSTR value);
	BOOL SetValue(LPCTSTR keyName, LPCTSTR valueName, DWORD value);
	BOOL SetValue(LPCTSTR keyName, LPCTSTR valueName, FLOAT value);

	BOOL GetValue(LPCTSTR keyName, LPCTSTR valueName, LPTSTR value, LPDWORD bufferLength);
	BOOL GetValue(LPCTSTR keyName, LPCTSTR valueName, LPDWORD value);
	BOOL GetValue(LPCTSTR keyName, LPCTSTR valueName, FLOAT* value);

private:
	TCHAR mFileName[MAX_PATH];
};

