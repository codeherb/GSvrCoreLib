#pragma once

#define MAX_BUFFER_LENGTH 2048

class CLog
{
public:
	static BOOL WriteLog(LPTSTR data, ...)
	{
		SYSTEMTIME systemTime;
		TCHAR currentDate[32]					= {0,};
		TCHAR currentFileName[MAX_PATH]			= {0,};
		FILE* filePtr							= NULL;
		TCHAR	debugLog[MAX_BUFFER_LENGTH]		= {0,};
		// 가변인자를 받기 위한 포인터 변수
		va_list ap;
		TCHAR	log[MAX_BUFFER_LENGTH]			= {0,};

		va_start(ap, data);			// 가변인자 포인터 변수 초기화
		// 버퍼에 가변인자로 받은 스트링 값을 출력
		_vstprintf(log, data, ap);	// 2버째 인자 : 스트링 및 포맷, 3번째 인자: 가변인자 
		va_end(ap);					// 가변인자 포인터 사용 종료

		GetLocalTime(&systemTime);
		// 길이 제한 버퍼 출력 함수
		_sntprintf(currentDate, 32, _T("%d-%d-%d %d:%d:%d"),
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond);

		_sntprintf(currentFileName, MAX_PATH, _T("LOG_%d-%d-%d %d.log"),
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour);

		filePtr = _tfopen(currentFileName, _T("a"));
		if(!filePtr)
			return FALSE;

		_ftprintf(filePtr, _T("[%s] %s\n"), currentDate, log);
		_sntprintf(debugLog, MAX_BUFFER_LENGTH, _T("[%s] %s\n"), currentDate, log);

		fflush(filePtr);
		fclose(filePtr);

		OutputDebugString(debugLog);
		_tprintf(_T("%s"), debugLog);

		return TRUE;

	}

	CLog(void)
	{
	}

	~CLog(void)
	{
	}
};

