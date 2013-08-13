#include "stdafx.h"
#include "Service.h"

/* Ŭ���� �ɹ� �Լ��� ȣ�� �Ծ��� �ڵ����� thiscall�� ������ �Ǵµ�
 * �̴� WINAPI�󿡼� �䱸�ϴ� cdecl�� ���� �ʾ� ���� ���� ���ϰ� ������
 * ������ �߻� ��Ű�µ� �̸� ��ȸ�ϱ� ���� Service.cpp���� ��������
 * ���� Ŭ������ �ν��Ͻ��� ����Ű�� �۷ι� ������ �����ϰ�
 * ������ ȣ��� �Ҵ��� �ش� �����͸� ���� ���� Ŭ���� ���� ó�� ������
 * ��ġ�� �Ѵ�.
 */
CService *gService = NULL;

VOID WINAPI RunCallback(DWORD argumentCount,LPWSTR *arguments) 
{
	if(!gService)
		return;

	gService->RunCallback(argumentCount, arguments);
}

VOID WINAPI CtrlHandlerCallback(DWORD opCode) 
{
	if(!gService)
		return;

	gService->CtrlHandlerCallback(opCode);
}

CService::CService(void)
{
	ZeroMemory(mServiceName, sizeof(mServiceName));
	gService = this;
}


CService::~CService(void)
{
}


BOOL CService::Install(LPCTSTR serviceName)
{
	TCHAR ServiceFileName[MAX_PATH] = {0,};
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE ServiceHandle = NULL;

	if(!serviceName)
		return FALSE;

	// File �н��� ū ����ǥ�� ���� ���·� ����� ����
	ServiceFileName[0] = '"';
	GetModuleFileName(NULL, ServiceFileName+1, MAX_PATH - 1);
	_tcscat(ServiceFileName, _T("\""));

	// SCM ����
	ServiceControlManager = OpenSCManager(NULL,NULL, SC_MANAGER_CREATE_SERVICE);
	if(!ServiceControlManager)
		return FALSE;

	// ���񽺸� SERVICE_WIN32_OWN_PROCESS ���·� �����Ѵ�.
	ServiceHandle = CreateService(ServiceControlManager,
		serviceName,	// ���� �̸�
		serviceName,	// �������� �̸��� �����ϰ�..
		SERVICE_ALL_ACCESS,	// ��� ������ �����ϰ�..
		SERVICE_WIN32_OWN_PROCESS,	// Win32 ������ ����
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		ServiceFileName,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if(!ServiceHandle) {
		CloseServiceHandle(ServiceControlManager);
		return FALSE;
	}

	// ����� �ڵ��� �ݾ��ش�.
	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceControlManager);

	return TRUE;
	
}


BOOL CService::Uninstall(LPCTSTR serviceName) {
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE ServiceHandle = NULL;

	if(!serviceName)
		return FALSE;

	ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(!ServiceControlManager)
		return FALSE;

	// Delete ������ ���� ���� �ڵ��� ����
	ServiceHandle = OpenService(ServiceControlManager, serviceName, DELETE);

	if(!ServiceHandle) {
		CloseServiceHandle(ServiceControlManager);
		return FALSE;
	}

	// ������ ���񽺸� ����
	if(!DeleteService(ServiceHandle)) {
		CloseServiceHandle(ServiceHandle);
		CloseServiceHandle(ServiceControlManager);

		return FALSE;
	}

	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceControlManager);

	return TRUE;
}


VOID CService::CtrlHandlerCallback(DWORD opCode)
{
	switch(opCode) {
	case SERVICE_CONTROL_PAUSE:
		mServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		mServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
		mServiceStatus.dwWin32ExitCode = 0;
		mServiceStatus.dwCurrentState = SERVICE_STOPPED;
		mServiceStatus.dwCheckPoint = 0;
		mServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	}

	// ���� ������ �Ǿ����Ƿ� ����
	if(!SetServiceStatus(mServiceStatusHandle, &mServiceStatus))
		return;
}

VOID CService::RunCallback(DWORD argumentCount, LPWSTR* arguments) 
{
	DWORD Status;

	mServiceStatus.dwServiceType = SERVICE_WIN32;	//Win32 ������ ����
	mServiceStatus.dwCurrentState = SERVICE_START_PENDING;	// ���۵��� ���� ����
	mServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	mServiceStatus.dwWin32ExitCode = 0;
	mServiceStatus.dwServiceSpecificExitCode = 0;
	mServiceStatus.dwCheckPoint = 0;
	mServiceStatus.dwWaitHint = 0;

	// ���¸� SERVICE_START_PENDING�� �س��� Control Callback�� ���
	mServiceStatusHandle = RegisterServiceCtrlHandler(mServiceName, ::CtrlHandlerCallback);

	if((SERVICE_STATUS_HANDLE)0  == mServiceStatusHandle)
		return;

	Status = NO_ERROR;

	// ��� ���������� SERVICE_RUNNING ���·� ����
	mServiceStatus.dwCurrentState = SERVICE_RUNNING;
	mServiceStatus.dwCheckPoint = 0;
	mServiceStatus.dwWaitHint = 0;

	if(!SetServiceStatus(mServiceStatusHandle, &mServiceStatus))
		return;

}


BOOL CService::Begin(LPTSTR serviceName)
{
	if(!serviceName)
		return FALSE;

	// StartServiceCtrlDispatcher���� ����� ���� ȯ�� ����
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{serviceName, ::RunCallback},
		{NULL, NULL}
	};

	_tcscpy(mServiceName, serviceName);

	OnStarted();

	if(!StartServiceCtrlDispatcher(DispatchTable)) {
		// ���� �϶�
		_tprintf(_T("## Debug Console mode ##\n"));
		getchar();
	}

	return TRUE;
}


BOOL CService::End(VOID)
{
	OnStopped();

	return TRUE;
}