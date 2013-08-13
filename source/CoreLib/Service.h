#pragma once

class CService
{
public:
	CService(void);
	virtual ~CService(void);

	BOOL Begin(LPTSTR serviceName);
	BOOL End(void);
	BOOL Install(LPCTSTR serviceName);
	BOOL Uninstall(LPCTSTR serviceName);

	// StargServiceCtrlDispatcher 함수에 의해 성공적으로 서비스가 실행됐을때 호출
	VOID WINAPI RunCallback(DWORD argumentCount, LPWSTR* arguments);
	// 서비스 상태 콜백
	VOID WINAPI CtrlHandlerCallback(DWORD opCode);

	virtual void OnStarted() = 0;
	virtual void OnStopped() = 0;

private :
	TCHAR mServiceName[MAX_PATH];
	SERVICE_STATUS mServiceStatus;
	SERVICE_STATUS_HANDLE mServiceStatusHandle;

};
