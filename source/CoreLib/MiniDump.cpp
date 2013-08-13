#include "StdAfx.h"
#include "MiniDump.h"

// 에러 메시지 출력 함수(인자값이 없는 경우 가장 마지막에 발생한 오류를 전달)
VOID CMiniDump::printLastError(DWORD error)
{
	TCHAR* lpOSMsg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpOSMsg, 0, NULL);

	_tprintf(_T("[ERROR] [%d] %s\n"), error, lpOSMsg);
	LocalFree(lpOSMsg);
}

//UnHandled Exception 발생시 호출될 콜백 함수
//스택 오버플로우 같은 현상이 나타날 경우에 MiniDump를 생성하기에 메모리가 부족한 경우가 생긴다
//이를 해결하기 위해 별도의 스래드를 활용하면 메모리 부족 현상 없이 생성이 가능하다.
LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
{
	// TODO
	if(EXCEPTION_STACK_OVERFLOW == exceptionInfo->ExceptionRecord->ExceptionCode) {
		// 스레드 생성Args (모름, 할당할 스택 사이즈, Run시킬 함수 포인터, 함수에 전달할 Argument, 모름, 모름);
		//HANDLE hThread = (HANDLE)_beginthreadex(0, 16777216, CreateMiniDump, &info, 0, NULL);
		// 덤프 생성이 종료 될때까지 대기
		//WaitForSingleObject(hThread, INFINITE);
		//CloseHandle(hThread);
		//return EXCEPTION_EXECUTE_HANDLER;
	}

	// DLL 로딩
	HMODULE DllHandle = LoadLibrary(_T("DBGHELP.DLL"));
	if(NULL == DllHandle) {
		CMiniDump::printLastError();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	//DBGHELP.DLL에서 MiniDumpWriteDump 프로시저 주소를 얻어와 Dump 함수 폰인터에 저장
	MINIDUMPWRITEDUMP Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");
	if(NULL == Dump) {
		CMiniDump::printLastError();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);		// 현재 시간 획득

	TCHAR DumpPath[MAX_PATH] = {0,};
	//덤프 파일 이름 설정
	_sntprintf_s(DumpPath, MAX_PATH, _TRUNCATE, _T("%d-%d-%d-%d-%d-%d.dmp"),
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond);

	// 덤프 파일 생성
	HANDLE FileHandle = CreateFile(DumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == FileHandle) {
		CMiniDump::printLastError();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	//MiniDump 예외 정보 구조체 셋팅
	_MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;
	MiniDumpExceptionInfo.ThreadId			= GetCurrentThreadId();
	MiniDumpExceptionInfo.ExceptionPointers = (PEXCEPTION_POINTERS)exceptionInfo;
	MiniDumpExceptionInfo.ClientPointers	= NULL;

	// 현재 프로세스에 대한 덤프 기록을 실행
	BOOL Success = Dump(GetCurrentProcess(),
		GetCurrentProcessId(),
		FileHandle,				// 덤프를 기록할 파일 핸들
		MiniDumpNormal,
		&MiniDumpExceptionInfo,	// MiniDump 예외 정보
		NULL,
		NULL);

	// 덤프 기록 성공
	if(Success) {
		CloseHandle(FileHandle);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	CloseHandle(FileHandle);
	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL CMiniDump::BeginDump(VOID)
{
	// 잘못된 연산 메시지 창이 나오지 않도록 설정
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// Unhandled Exception 발생시 호출될 CallBack 함수 등록
	// PreviousExceptionFilter는 원래 설정을 담아 놓았다 프로그램 복구시 사용
	PriviousExceptionFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)UnHandledExceptionFilter);

	return TRUE;
}

BOOL CMiniDump::EndDump(VOID)
{
	SetUnhandledExceptionFilter(PriviousExceptionFilter);

	return TRUE;
}
