#include "stdafx.h"
#include "Registry.h"


CRegistry::CRegistry(void)
{
	mRootKey = NULL;		// 사용할 레지스트리 키를 저장 하는 변수
	mIsOpened = false;		// 해당 레지스트리를 열었는지 체크하는 변수
}


CRegistry::~CRegistry(void)
{
}

// 처음 키를 열 때 사용한다.
BOOL CRegistry::Open(HKEY rootKey, LPCTSTR subKey)
{
	// 서브키가 널인지 체크
	if(!subKey)
		return FALSE;

	// 이미 열려있는 경우 실패를 날린다
	if(mIsOpened)
		return FALSE;

	// 지정된 키를 열어서 mRootKey변수에 저장
	if(ERROR_SUCCESS != RegOpenKey(rootKey, subKey, &mRootKey))
		return FALSE;

	// ERROR_SUCCESS이므로 키가 열려있음 상태로 플래그 셋팅
	mIsOpened = TRUE;

	return TRUE;
}

// 키를 닫을 때 사용
BOOL CRegistry::Close(void)
{
	if(ERROR_SUCCESS != RegCloseKey(mRootKey))
		return FALSE;

	// 키를 닫으면 플래그 값을 FALSE로 바꾼다
	mIsOpened = FALSE;

	return TRUE;
}

// 키를 생성할 때 사용
BOOL CRegistry::CreateKey(HKEY rootKey, LPCTSTR subKey)
{
	// rootKey를 기준으로 생성할 subKey를 입력받아 키를 생성한다.
	if(!subKey)
		return FALSE;

	// 이미 열려 있는 경우 실패
	if(mIsOpened)
		return FALSE;

	//레지스트리 키를 생성하는 API함수를 호출한다.
	if(ERROR_SUCCESS != RegCreateKey(rootKey, subKey, &mRootKey))
		return FALSE;

	// OpenKey 이외에 생성하면 자동으로 열린 형태로 셋팅된다.
	mIsOpened = TRUE;

	return TRUE;
}

// 키를 삭제하는 변수
BOOL CRegistry::DeleteKey(HKEY rootKey, LPCTSTR subKey)
{
	if(!subKey)
		return FALSE;

	// 레지스트리 키를 삭제하는 API 함수를 호출한다.
	if(ERROR_SUCCESS != RegDeleteKey(rootKey, subKey))
		return FALSE;

	return TRUE;
}

// 키와 달리 Create할 필요 없이 SetValue로 값이 없을때 셋팅 가능하다.
BOOL CRegistry::SetValue(LPCTSTR valueName, LPCTSTR value)
{
	// TCHAR형의 문자열 데이터를 레지스트리에 입력할 때 상용
	if(!valueName || !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// REG_SZ형으로 데이터를 입력한다.
	if(ERROR_SUCCESS != RegSetValueEx(mRootKey, valueName, 0, REG_SZ, (BYTE*)value, (DWORD)_tcslen(value) * sizeof(TCHAR)))
		return FALSE;

	return TRUE;
}

// MultiSZ형 문자열을 사용할때의 함수
BOOL CRegistry::SetValueForMultiSz(LPCTSTR valueName, LPCTSTR value, DWORD byteLength)
{
	// TCHAR형의 문자열 데이터를 레지스트리에 입력할 때 상용
	if(!valueName || !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// REG_SZ형으로 데이터를 입력한다.
	if(ERROR_SUCCESS != RegSetValueEx(mRootKey, valueName, 0, REG_MULTI_SZ, (BYTE*)value, byteLength))
		return FALSE;

	return TRUE;
}

// DWORD형 값을 저장하는 함수
BOOL CRegistry::SetValue(LPCTSTR valueName, DWORD value)
{
	// TCHAR형의 문자열 데이터를 레지스트리에 입력할 때 상용
	if(!valueName || !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// REG_SZ형으로 데이터를 입력한다.
	if(ERROR_SUCCESS != RegSetValueEx(mRootKey, valueName, 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD)))
		return FALSE;

	return TRUE;
}

// TCHAR형의 문자열 데이터를 읽어오는 함수
BOOL CRegistry::GetValue(LPCTSTR valueName, LPCTSTR value, LPDWORD bufferLength)
{
	DWORD valueType = 0;

	if(!valueName | !value | !bufferLength)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// 문자열 데이터를 bufferLength에 저장된 값만큼 읽는다
	if(ERROR_SUCCESS != RegQueryValueEx(mRootKey, valueName, 0, &valueType, (BYTE*)value, bufferLength))
		return FALSE;

	return TRUE;
}

// DWORD형 데이터를 읽어온다.
BOOL CRegistry::GetValue(LPCTSTR valueName, LPDWORD value)
{
	DWORD valueType = 0;
	DWORD bufferLength = sizeof(DWORD);

	if(!valueName | !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// 문자열 데이터를 bufferLength에 저장된 값만큼 읽는다
	if(ERROR_SUCCESS != RegQueryValueEx(mRootKey, valueName, 0, &valueType, (BYTE*)value, &bufferLength))
		return FALSE;

	return TRUE;
}

// 값을 삭제하는 함수
BOOL CRegistry::DeleteValue(LPCTSTR valueName)
{
	if(!valueName)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	if(ERROR_SUCCESS != RegDeleteValue(mRootKey, valueName))
		return FALSE;

	return TRUE;
}