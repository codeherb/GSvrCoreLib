#include "StdAfx.h"
#include "Iocp.h"

DWORD WINAPI WorkerThreadCallback(LPVOID parameter)
{

	CIocp *Owner = (CIocp*) parameter;
	Owner->WorkerThreadCallback();

	return 0;

}

CIocp::CIocp(void)
{
	mIocpHandle = NULL;			// IOCP 관리 핸들값
	mWorkerThreadCount = 0;		// 사용할 워커 스래드 갯수

	mStartupEventHandle = NULL;	// 시작을 관리하는 이벤트 핸들
}


CIocp::~CIocp(void)
{
}



BOOL CIocp::Begin(void)
{
	mIocpHandle = NULL;

	// 시스템 정보를 가져온다(CPU 정보를 활용)
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	// CPU갯수의 2배 만큼 워크 스래드 생성
	mWorkerThreadCount = systemInfo.dwNumberOfProcessors * 2;

	//IOCP 핸들을 생성
	//메인 IOCP핸들의 생성이므로 모든 파라미터의 값이 0혹은 NULL
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if(!mIocpHandle)
		return FALSE;

	// 시작을 관리하는 이벤트도 생성
	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if(!mStartupEventHandle) {
		End();
		return FALSE;
	}

	// CPU갯수의 2배 만큼의 WorkerThread를 생성해서 mWorkerThreadVector로 관리
	for(DWORD i = 0; i < mWorkerThreadCount; i++) {
		HANDLE workerThread = CreateThread(NULL, 0, ::WorkerThreadCallback, this, 0, NULL);
		mWorkerThreadVector.push_back(workerThread);
		WaitForSingleObject(mStartupEventHandle, INFINITE);
	} // end for

	return TRUE;
}

// IOCP에 소캣이나 파일 핸들을 등록하는 함수
BOOL CIocp::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{
	if(!socket || !completionKey)
		return FALSE;

	// Begin에서 생성했던 mIocpHandle에 추가로 소캣 핸들 및 키를 등록
	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);

	if(!mIocpHandle)
		return FALSE;

	return TRUE;
}

void CIocp::WorkerThreadCallback(void)
{
	// 성공 실패 유무 판단하기 위한 변수
	BOOL successed = FALSE;
	DWORD numberOfByteTransfered = 0;	// 몇 바이트의 IO가 발생했는지 확인
	void* completionKey = NULL;			// 키 값
	OVERLAPPED* overlapped = NULL;
	OVERLAPPED_EX* overlappedEx = NULL;
	void* object = NULL;

	while(TRUE) {
		SetEvent(mStartupEventHandle);

		// PostQueuedCompletionStatus나 IOCP에 등록된 소캣 및 파일의 Read, Write
		// 관련 함수의 상태를 받아오는 함수
		successed = GetQueuedCompletionStatus(mIocpHandle, &numberOfByteTransfered, (LPDWORD) &completionKey, &overlapped, INFINITE);

		//키가 NULL인 경우 종료로 데이터가 잘못 넘어왔거나 END에서 PostQueuedCompletionStatus를 통해 셋팅된다
		if(!completionKey)
			return;

		overlappedEx = (OVERLAPPED_EX*)overlapped;

		object = overlappedEx->object;

		if(!successed || (successed && !numberOfByteTransfered)) {
			if(overlappedEx->ioType == IO_ACCEPT)
				OnIOConnected(object);
			else
				OnIODisconnected(object);

			continue;
		}

		switch(overlappedEx->ioType) {
		case IO_READ:
			OnIORead(object, numberOfByteTransfered);
			break;
		case IO_WRITE:
			OnIOWrite(object, numberOfByteTransfered);
			break;
		} // end switch

	} // end while
}