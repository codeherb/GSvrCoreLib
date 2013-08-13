#pragma once

#include <Windows.h>

#include <DbgHelp.h>

// Unhandled Exception 발생시 호출될 CallBack 함수 원형
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)
(
	HANDLE	hProcess,
	DWORD	dwPid,
	HANDLE	hFile,
	MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

class CMiniDump
{
private:
	static LPTOP_LEVEL_EXCEPTION_FILTER PriviousExceptionFilter;

public:
	static BOOL BeginDump(VOID);
	static BOOL EndDump(VOID);

	static VOID printLastError(DWORD error = GetLastError());
};

