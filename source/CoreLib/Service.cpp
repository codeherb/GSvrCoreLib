#include "stdafx.h"
#include "Service.h"

/* 클래스 맴버 함수는 호출 규약이 자동으로 thiscall을 가지게 되는데
 * 이는 WINAPI상에서 요구하는 cdecl과 맞지 않아 전달 하지 못하고 컴파일
 * 오류를 발생 시키는데 이를 우회하기 위해 Service.cpp에만 종속적인
 * 서비스 클래스의 인스턴스를 가르키는 글로벌 변수를 선언하고
 * 생성자 호출시 할당후 해당 포인터를 통해 서비스 클래스 안의 처리 로직을
 * 거치게 한다.
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

	// File 패스를 큰 따옴표로 쌓인 형태로 만들어 저장
	ServiceFileName[0] = '"';
	GetModuleFileName(NULL, ServiceFileName+1, MAX_PATH - 1);
	_tcscat(ServiceFileName, _T("\""));

	// SCM 열기
	ServiceControlManager = OpenSCManager(NULL,NULL, SC_MANAGER_CREATE_SERVICE);
	if(!ServiceControlManager)
		return FALSE;

	// 서비스를 SERVICE_WIN32_OWN_PROCESS 형태로 생성한다.
	ServiceHandle = CreateService(ServiceControlManager,
		serviceName,	// 서비스 이름
		serviceName,	// 보여지는 이름도 동일하게..
		SERVICE_ALL_ACCESS,	// 모든 엑세스 가능하게..
		SERVICE_WIN32_OWN_PROCESS,	// Win32 형태의 파일
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

	// 사용한 핸들은 닫아준다.
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

	// Delete 권한을 가진 서비스 핸들을 얻어옴
	ServiceHandle = OpenService(ServiceControlManager, serviceName, DELETE);

	if(!ServiceHandle) {
		CloseServiceHandle(ServiceControlManager);
		return FALSE;
	}

	// 실제로 서비스를 삭제
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

	// 상태 변경이 되었으므로 적용
	if(!SetServiceStatus(mServiceStatusHandle, &mServiceStatus))
		return;
}

VOID CService::RunCallback(DWORD argumentCount, LPWSTR* arguments) 
{
	DWORD Status;

	mServiceStatus.dwServiceType = SERVICE_WIN32;	//Win32 형태의 서비스
	mServiceStatus.dwCurrentState = SERVICE_START_PENDING;	// 시작되지 않은 상태
	mServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	mServiceStatus.dwWin32ExitCode = 0;
	mServiceStatus.dwServiceSpecificExitCode = 0;
	mServiceStatus.dwCheckPoint = 0;
	mServiceStatus.dwWaitHint = 0;

	// 상태를 SERVICE_START_PENDING로 해놓고 Control Callback을 등록
	mServiceStatusHandle = RegisterServiceCtrlHandler(mServiceName, ::CtrlHandlerCallback);

	if((SERVICE_STATUS_HANDLE)0  == mServiceStatusHandle)
		return;

	Status = NO_ERROR;

	// 등록 성공했을때 SERVICE_RUNNING 상태로 변경
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

	// StartServiceCtrlDispatcher에서 등록할 서비스 환경 정보
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{serviceName, ::RunCallback},
		{NULL, NULL}
	};

	_tcscpy(mServiceName, serviceName);

	OnStarted();

	if(!StartServiceCtrlDispatcher(DispatchTable)) {
		// 실패 일때
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