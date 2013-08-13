#include "StdAfx.h"
#include "MiniDump.h"

// ���� �޽��� ��� �Լ�(���ڰ��� ���� ��� ���� �������� �߻��� ������ ����)
VOID CMiniDump::printLastError(DWORD error)
{
	TCHAR* lpOSMsg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpOSMsg, 0, NULL);

	_tprintf(_T("[ERROR] [%d] %s\n"), error, lpOSMsg);
	LocalFree(lpOSMsg);
}

//UnHandled Exception �߻��� ȣ��� �ݹ� �Լ�
//���� �����÷ο� ���� ������ ��Ÿ�� ��쿡 MiniDump�� �����ϱ⿡ �޸𸮰� ������ ��찡 �����
//�̸� �ذ��ϱ� ���� ������ �����带 Ȱ���ϸ� �޸� ���� ���� ���� ������ �����ϴ�.
LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
{
	// TODO
	if(EXCEPTION_STACK_OVERFLOW == exceptionInfo->ExceptionRecord->ExceptionCode) {
		// ������ ����Args (��, �Ҵ��� ���� ������, Run��ų �Լ� ������, �Լ��� ������ Argument, ��, ��);
		//HANDLE hThread = (HANDLE)_beginthreadex(0, 16777216, CreateMiniDump, &info, 0, NULL);
		// ���� ������ ���� �ɶ����� ���
		//WaitForSingleObject(hThread, INFINITE);
		//CloseHandle(hThread);
		//return EXCEPTION_EXECUTE_HANDLER;
	}

	// DLL �ε�
	HMODULE DllHandle = LoadLibrary(_T("DBGHELP.DLL"));
	if(NULL == DllHandle) {
		CMiniDump::printLastError();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	//DBGHELP.DLL���� MiniDumpWriteDump ���ν��� �ּҸ� ���� Dump �Լ� �����Ϳ� ����
	MINIDUMPWRITEDUMP Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");
	if(NULL == Dump) {
		CMiniDump::printLastError();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);		// ���� �ð� ȹ��

	TCHAR DumpPath[MAX_PATH] = {0,};
	//���� ���� �̸� ����
	_sntprintf_s(DumpPath, MAX_PATH, _TRUNCATE, _T("%d-%d-%d-%d-%d-%d.dmp"),
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond);

	// ���� ���� ����
	HANDLE FileHandle = CreateFile(DumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == FileHandle) {
		CMiniDump::printLastError();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	//MiniDump ���� ���� ����ü ����
	_MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;
	MiniDumpExceptionInfo.ThreadId			= GetCurrentThreadId();
	MiniDumpExceptionInfo.ExceptionPointers = (PEXCEPTION_POINTERS)exceptionInfo;
	MiniDumpExceptionInfo.ClientPointers	= NULL;

	// ���� ���μ����� ���� ���� ����� ����
	BOOL Success = Dump(GetCurrentProcess(),
		GetCurrentProcessId(),
		FileHandle,				// ������ ����� ���� �ڵ�
		MiniDumpNormal,
		&MiniDumpExceptionInfo,	// MiniDump ���� ����
		NULL,
		NULL);

	// ���� ��� ����
	if(Success) {
		CloseHandle(FileHandle);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	CloseHandle(FileHandle);
	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL CMiniDump::BeginDump(VOID)
{
	// �߸��� ���� �޽��� â�� ������ �ʵ��� ����
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// Unhandled Exception �߻��� ȣ��� CallBack �Լ� ���
	// PreviousExceptionFilter�� ���� ������ ��� ���Ҵ� ���α׷� ������ ���
	PriviousExceptionFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)UnHandledExceptionFilter);

	return TRUE;
}

BOOL CMiniDump::EndDump(VOID)
{
	SetUnhandledExceptionFilter(PriviousExceptionFilter);

	return TRUE;
}
