#include "stdafx.h"
#include "Registry.h"


CRegistry::CRegistry(void)
{
	mRootKey = NULL;		// ����� ������Ʈ�� Ű�� ���� �ϴ� ����
	mIsOpened = false;		// �ش� ������Ʈ���� �������� üũ�ϴ� ����
}


CRegistry::~CRegistry(void)
{
}

// ó�� Ű�� �� �� ����Ѵ�.
BOOL CRegistry::Open(HKEY rootKey, LPCTSTR subKey)
{
	// ����Ű�� ������ üũ
	if(!subKey)
		return FALSE;

	// �̹� �����ִ� ��� ���и� ������
	if(mIsOpened)
		return FALSE;

	// ������ Ű�� ��� mRootKey������ ����
	if(ERROR_SUCCESS != RegOpenKey(rootKey, subKey, &mRootKey))
		return FALSE;

	// ERROR_SUCCESS�̹Ƿ� Ű�� �������� ���·� �÷��� ����
	mIsOpened = TRUE;

	return TRUE;
}

// Ű�� ���� �� ���
BOOL CRegistry::Close(void)
{
	if(ERROR_SUCCESS != RegCloseKey(mRootKey))
		return FALSE;

	// Ű�� ������ �÷��� ���� FALSE�� �ٲ۴�
	mIsOpened = FALSE;

	return TRUE;
}

// Ű�� ������ �� ���
BOOL CRegistry::CreateKey(HKEY rootKey, LPCTSTR subKey)
{
	// rootKey�� �������� ������ subKey�� �Է¹޾� Ű�� �����Ѵ�.
	if(!subKey)
		return FALSE;

	// �̹� ���� �ִ� ��� ����
	if(mIsOpened)
		return FALSE;

	//������Ʈ�� Ű�� �����ϴ� API�Լ��� ȣ���Ѵ�.
	if(ERROR_SUCCESS != RegCreateKey(rootKey, subKey, &mRootKey))
		return FALSE;

	// OpenKey �̿ܿ� �����ϸ� �ڵ����� ���� ���·� ���õȴ�.
	mIsOpened = TRUE;

	return TRUE;
}

// Ű�� �����ϴ� ����
BOOL CRegistry::DeleteKey(HKEY rootKey, LPCTSTR subKey)
{
	if(!subKey)
		return FALSE;

	// ������Ʈ�� Ű�� �����ϴ� API �Լ��� ȣ���Ѵ�.
	if(ERROR_SUCCESS != RegDeleteKey(rootKey, subKey))
		return FALSE;

	return TRUE;
}

// Ű�� �޸� Create�� �ʿ� ���� SetValue�� ���� ������ ���� �����ϴ�.
BOOL CRegistry::SetValue(LPCTSTR valueName, LPCTSTR value)
{
	// TCHAR���� ���ڿ� �����͸� ������Ʈ���� �Է��� �� ���
	if(!valueName || !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// REG_SZ������ �����͸� �Է��Ѵ�.
	if(ERROR_SUCCESS != RegSetValueEx(mRootKey, valueName, 0, REG_SZ, (BYTE*)value, (DWORD)_tcslen(value) * sizeof(TCHAR)))
		return FALSE;

	return TRUE;
}

// MultiSZ�� ���ڿ��� ����Ҷ��� �Լ�
BOOL CRegistry::SetValueForMultiSz(LPCTSTR valueName, LPCTSTR value, DWORD byteLength)
{
	// TCHAR���� ���ڿ� �����͸� ������Ʈ���� �Է��� �� ���
	if(!valueName || !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// REG_SZ������ �����͸� �Է��Ѵ�.
	if(ERROR_SUCCESS != RegSetValueEx(mRootKey, valueName, 0, REG_MULTI_SZ, (BYTE*)value, byteLength))
		return FALSE;

	return TRUE;
}

// DWORD�� ���� �����ϴ� �Լ�
BOOL CRegistry::SetValue(LPCTSTR valueName, DWORD value)
{
	// TCHAR���� ���ڿ� �����͸� ������Ʈ���� �Է��� �� ���
	if(!valueName || !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// REG_SZ������ �����͸� �Է��Ѵ�.
	if(ERROR_SUCCESS != RegSetValueEx(mRootKey, valueName, 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD)))
		return FALSE;

	return TRUE;
}

// TCHAR���� ���ڿ� �����͸� �о���� �Լ�
BOOL CRegistry::GetValue(LPCTSTR valueName, LPCTSTR value, LPDWORD bufferLength)
{
	DWORD valueType = 0;

	if(!valueName | !value | !bufferLength)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// ���ڿ� �����͸� bufferLength�� ����� ����ŭ �д´�
	if(ERROR_SUCCESS != RegQueryValueEx(mRootKey, valueName, 0, &valueType, (BYTE*)value, bufferLength))
		return FALSE;

	return TRUE;
}

// DWORD�� �����͸� �о�´�.
BOOL CRegistry::GetValue(LPCTSTR valueName, LPDWORD value)
{
	DWORD valueType = 0;
	DWORD bufferLength = sizeof(DWORD);

	if(!valueName | !value)
		return FALSE;

	if(!mIsOpened)
		return FALSE;

	// ���ڿ� �����͸� bufferLength�� ����� ����ŭ �д´�
	if(ERROR_SUCCESS != RegQueryValueEx(mRootKey, valueName, 0, &valueType, (BYTE*)value, &bufferLength))
		return FALSE;

	return TRUE;
}

// ���� �����ϴ� �Լ�
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