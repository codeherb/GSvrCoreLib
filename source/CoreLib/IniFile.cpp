#include "stdafx.h"
#include "IniFile.h"

CIniFile::CIniFile(void)
{
	ZeroMemory(mFileName, sizeof(mFileName));
}


CIniFile::~CIniFile(void)
{
}

// Ini������ �����ϴ� �Լ� �Ķ���ʹ� TCHAR������ �޴´�.
BOOL CIniFile::Open(LPCTSTR fileName)
{
	if(!fileName)
		return FALSE;

	// MultiByte(ANSI)�� �� ���� strcpy(), UniCode�� ���� wcscpy()ȣ���ϴ� ��ũ��
	_tcsncpy(mFileName, fileName, MAX_PATH);

	return TRUE;
}

BOOL CIniFile::Close(void)
{
	return TRUE;
}

/** DWORD���� �������� �Լ�
 * keyName : ���� �̸�
 * valueName : Ű��
 * value     : ���
 */
BOOL CIniFile::GetValue(LPCTSTR keyName, LPCTSTR valueName, LPDWORD value)
{
	if(!keyName || !valueName || !value)
		return FALSE;

	*value = GetPrivateProfileInt(keyName, valueName, 0, mFileName);

	return TRUE;
}

/** FLOAT���� �������� �Լ�
 * keyName : ���� �̸�
 * valueName : Ű��
 * value     : ���
 */
BOOL CIniFile::GetValue(LPCTSTR keyName, LPCTSTR valueName, FLOAT* value)
{
	if(!keyName || !valueName || !value)
		return FALSE;

	TCHAR Value[16] = {0,};

	//FLOAT���� ó���Ҽ� �ִ� API�Լ��� �����Ƿ� String���� ó�� ���� ��ȯ
	GetPrivateProfileString(keyName, valueName, _T(""), Value, 16, mFileName);

	*value = (FLOAT)_tstof(Value);

	return TRUE;
}

/** ���̳ʸ� ���� ���ڿ����� �������� �Լ�
 * keyName : ���� �̸�
 * valueName : Ű��
 * value     : ���
 * bufferLength : ���� ����
 */
BOOL CIniFile::GetValue(LPCTSTR keyName, LPCTSTR valueName, LPTSTR value, LPDWORD bufferLength)
{
	if(!keyName || !valueName || !value || !bufferLength)
		return FALSE;

	*bufferLength = GetPrivateProfileString(keyName, valueName, _T(""), value, *bufferLength, mFileName);

	return TRUE;
}

/** DWORD���� ���� �Լ�
 * keyName : ���� �̸�
 * valueName : Ű��
 * value     : ���
 */
BOOL CIniFile::SetValue(LPCTSTR keyName, LPCTSTR valueName, DWORD value)
{
	if(!keyName || !valueName)
		return FALSE;

	// ����ȯ �Լ��� ����� ��Ʈ�������� ��ȯ (������(4byte)�� 10������ ǥ�� �� ��� 11�ڸ��� ���� �����Ƿ� 16���� ����)
	TCHAR Value[16] = {0,};

	// _itot(src:value, dst:str*, ����) �ش��ϴ� ������ ���ڸ� �ؽ�Ʈ�� ��ȯ
	_itot(value, Value, 10);

	WritePrivateProfileString(keyName, valueName, Value, mFileName);

	return TRUE;
}

/** FLOAT���� ���� �Լ�
 * keyName : ���� �̸�
 * valueName : Ű��
 * value     : ���
 */
BOOL CIniFile::SetValue(LPCTSTR keyName, LPCTSTR valueName, FLOAT value)
{
	if(!keyName || !valueName)
		return FALSE;

	// ����ȯ �Լ��� ����� ��Ʈ�������� ��ȯ (FLOAT��(4byte)�� 10������ ǥ�� �� ��� �ſ� �� ������ ���ڿ��� ������ ���������� ������ ����)
	TCHAR Value[16] = {0,};
	_stprintf(Value, _T("%f"), value);

	WritePrivateProfileString(keyName, valueName, Value, mFileName);

	return TRUE;
}

/** ���ڿ����� ���� �Լ�
 * keyName : ���� �̸�
 * valueName : Ű��
 * value     : ���
 */
BOOL CIniFile::SetValue(LPCTSTR keyName, LPCTSTR valueName, LPCTSTR value)
{
	if(!keyName || !valueName || !value)
		return FALSE;

	WritePrivateProfileString(keyName, valueName, value, mFileName);

	return TRUE;
}