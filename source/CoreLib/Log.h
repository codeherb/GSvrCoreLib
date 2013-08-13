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
		// �������ڸ� �ޱ� ���� ������ ����
		va_list ap;
		TCHAR	log[MAX_BUFFER_LENGTH]			= {0,};

		va_start(ap, data);			// �������� ������ ���� �ʱ�ȭ
		// ���ۿ� �������ڷ� ���� ��Ʈ�� ���� ���
		_vstprintf(log, data, ap);	// 2��° ���� : ��Ʈ�� �� ����, 3��° ����: �������� 
		va_end(ap);					// �������� ������ ��� ����

		GetLocalTime(&systemTime);
		// ���� ���� ���� ��� �Լ�
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

