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
	mIocpHandle = NULL;			// IOCP ���� �ڵ鰪
	mWorkerThreadCount = 0;		// ����� ��Ŀ ������ ����

	mStartupEventHandle = NULL;	// ������ �����ϴ� �̺�Ʈ �ڵ�
}


CIocp::~CIocp(void)
{
}



BOOL CIocp::Begin(void)
{
	mIocpHandle = NULL;

	// �ý��� ������ �����´�(CPU ������ Ȱ��)
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	// CPU������ 2�� ��ŭ ��ũ ������ ����
	mWorkerThreadCount = systemInfo.dwNumberOfProcessors * 2;

	//IOCP �ڵ��� ����
	//���� IOCP�ڵ��� �����̹Ƿ� ��� �Ķ������ ���� 0Ȥ�� NULL
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if(!mIocpHandle)
		return FALSE;

	// ������ �����ϴ� �̺�Ʈ�� ����
	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if(!mStartupEventHandle) {
		End();
		return FALSE;
	}

	// CPU������ 2�� ��ŭ�� WorkerThread�� �����ؼ� mWorkerThreadVector�� ����
	for(DWORD i = 0; i < mWorkerThreadCount; i++) {
		HANDLE workerThread = CreateThread(NULL, 0, ::WorkerThreadCallback, this, 0, NULL);
		mWorkerThreadVector.push_back(workerThread);
		WaitForSingleObject(mStartupEventHandle, INFINITE);
	} // end for

	return TRUE;
}

// IOCP�� ��Ĺ�̳� ���� �ڵ��� ����ϴ� �Լ�
BOOL CIocp::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{
	if(!socket || !completionKey)
		return FALSE;

	// Begin���� �����ߴ� mIocpHandle�� �߰��� ��Ĺ �ڵ� �� Ű�� ���
	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);

	if(!mIocpHandle)
		return FALSE;

	return TRUE;
}

void CIocp::WorkerThreadCallback(void)
{
	// ���� ���� ���� �Ǵ��ϱ� ���� ����
	BOOL successed = FALSE;
	DWORD numberOfByteTransfered = 0;	// �� ����Ʈ�� IO�� �߻��ߴ��� Ȯ��
	void* completionKey = NULL;			// Ű ��
	OVERLAPPED* overlapped = NULL;
	OVERLAPPED_EX* overlappedEx = NULL;
	void* object = NULL;

	while(TRUE) {
		SetEvent(mStartupEventHandle);

		// PostQueuedCompletionStatus�� IOCP�� ��ϵ� ��Ĺ �� ������ Read, Write
		// ���� �Լ��� ���¸� �޾ƿ��� �Լ�
		successed = GetQueuedCompletionStatus(mIocpHandle, &numberOfByteTransfered, (LPDWORD) &completionKey, &overlapped, INFINITE);

		//Ű�� NULL�� ��� ����� �����Ͱ� �߸� �Ѿ�԰ų� END���� PostQueuedCompletionStatus�� ���� ���õȴ�
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