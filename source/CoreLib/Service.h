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

	// StargServiceCtrlDispatcher �Լ��� ���� ���������� ���񽺰� ��������� ȣ��
	VOID WINAPI RunCallback(DWORD argumentCount, LPWSTR* arguments);
	// ���� ���� �ݹ�
	VOID WINAPI CtrlHandlerCallback(DWORD opCode);

	virtual void OnStarted() = 0;
	virtual void OnStopped() = 0;

private :
	TCHAR mServiceName[MAX_PATH];
	SERVICE_STATUS mServiceStatus;
	SERVICE_STATUS_HANDLE mServiceStatusHandle;

};
