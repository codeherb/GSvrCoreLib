#include "stdafx.h"
#include "IniFile.h"

CIniFile::CIniFile(void)
{
	ZeroMemory(mFileName, sizeof(mFileName));
}


CIniFile::~CIniFile(void)
{
}

// Ini파일을 오픈하는 함수 파라미터는 TCHAR형으로 받는다.
BOOL CIniFile::Open(LPCTSTR fileName)
{
	if(!fileName)
		return FALSE;

	// MultiByte(ANSI)로 된 경우는 strcpy(), UniCode인 경우는 wcscpy()호출하는 매크로
	_tcsncpy(mFileName, fileName, MAX_PATH);

	return TRUE;
}

BOOL CIniFile::Close(void)
{
	return TRUE;
}

/** DWORD값을 가져오는 함수
 * keyName : 세션 이름
 * valueName : 키값
 * value     : 밸류
 */
BOOL CIniFile::GetValue(LPCTSTR keyName, LPCTSTR valueName, LPDWORD value)
{
	if(!keyName || !valueName || !value)
		return FALSE;

	*value = GetPrivateProfileInt(keyName, valueName, 0, mFileName);

	return TRUE;
}

/** FLOAT값을 가져오는 함수
 * keyName : 세션 이름
 * valueName : 키값
 * value     : 밸류
 */
BOOL CIniFile::GetValue(LPCTSTR keyName, LPCTSTR valueName, FLOAT* value)
{
	if(!keyName || !valueName || !value)
		return FALSE;

	TCHAR Value[16] = {0,};

	//FLOAT형을 처리할수 있는 API함수가 없으므로 String으로 처리 한후 변환
	GetPrivateProfileString(keyName, valueName, _T(""), Value, 16, mFileName);

	*value = (FLOAT)_tstof(Value);

	return TRUE;
}

/** 바이너리 값과 문자열값을 가져오는 함수
 * keyName : 세션 이름
 * valueName : 키값
 * value     : 밸류
 * bufferLength : 버퍼 길이
 */
BOOL CIniFile::GetValue(LPCTSTR keyName, LPCTSTR valueName, LPTSTR value, LPDWORD bufferLength)
{
	if(!keyName || !valueName || !value || !bufferLength)
		return FALSE;

	*bufferLength = GetPrivateProfileString(keyName, valueName, _T(""), value, *bufferLength, mFileName);

	return TRUE;
}

/** DWORD값을 쓰는 함수
 * keyName : 세션 이름
 * valueName : 키값
 * value     : 밸류
 */
BOOL CIniFile::SetValue(LPCTSTR keyName, LPCTSTR valueName, DWORD value)
{
	if(!keyName || !valueName)
		return FALSE;

	// 형변환 함수를 사용해 스트링형으로 변환 (정수형(4byte)을 10진수로 표현 할 경우 11자리를 넘지 않으므로 16으로 셋팅)
	TCHAR Value[16] = {0,};

	// _itot(src:value, dst:str*, 진수) 해당하는 진법의 숫자를 텍스트로 변환
	_itot(value, Value, 10);

	WritePrivateProfileString(keyName, valueName, Value, mFileName);

	return TRUE;
}

/** FLOAT값을 쓰는 함수
 * keyName : 세션 이름
 * valueName : 키값
 * value     : 밸류
 */
BOOL CIniFile::SetValue(LPCTSTR keyName, LPCTSTR valueName, FLOAT value)
{
	if(!keyName || !valueName)
		return FALSE;

	// 형변환 함수를 사용해 스트링형으로 변환 (FLOAT형(4byte)을 10진수로 표현 할 경우 매우 긴 길이의 문자열이 나오나 그정도까지 쓸일이 없음)
	TCHAR Value[16] = {0,};
	_stprintf(Value, _T("%f"), value);

	WritePrivateProfileString(keyName, valueName, Value, mFileName);

	return TRUE;
}

/** 문자열값을 쓰는 함수
 * keyName : 세션 이름
 * valueName : 키값
 * value     : 밸류
 */
BOOL CIniFile::SetValue(LPCTSTR keyName, LPCTSTR valueName, LPCTSTR value)
{
	if(!keyName || !valueName || !value)
		return FALSE;

	WritePrivateProfileString(keyName, valueName, value, mFileName);

	return TRUE;
}