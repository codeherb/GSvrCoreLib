#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkSession.h"

void WINAPI ReliableUdpThreadCallback() 
{
}

CNetworkSession::CNetworkSession(void)
{
	/* Overlapped����ü�� IO ������Ʈ�� ���� ���°��� �����ϴ� �ڷᱸ���̴�.
	 * ������ OVERLAPPED_EX�� �̰��� � ���� �ϴ� �������� ��Ȯ�ϰ� �˱� ����
	 * ����� ���� ����ü�� ���ο� Overlapped ������ ������ �ִ�. */

	// Accept ���� Overlapped����ü
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	// Read ���� Overlapped����ü
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	// Write ���� Overlapped����ü
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	// ��Ŷ�� �޾Ҵٴ� ��ȣ�� ���� ������ ���� ��Ŷ�� ����������� ������ �ڵ�
	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;	// ������ ���� �̺�Ʈ
	mReliableUdpThreadDestroyEvent = NULL;	// ������ ���� �̺�Ʈ
	mReliableUdpThreadWakeUpEvent = NULL;	// ������ ����� �̺�Ʈ
	mReliableUdpWriteCompleteEvent = NULL;	// ��Ŷ�� ���̻� ���� �ʿ䰡 ������ ������ �̺�Ʈ

	mIsReliableUdpSending = FALSE;

	mAcceptOverlapped.ioType	= IO_ACCEPT;
	mReadOverlapped.ioType		= IO_READ;
	mWriteOverlapped.ioType		= IO_WRITE;

	mAcceptOverlapped.object	= this;
	mReadOverlapped.object		= this;
	mWriteOverlapped.object		= this;
}


CNetworkSession::~CNetworkSession(void)
{
}

BOOL CNetworkSession::Begin(void)
{
	CThreadSync sync; // ���� ������ ����ȭ

	if(mSocket) // �������� ������ ������ ����
		return FALSE;

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;
	mReliableUdpThreadDestroyEvent = NULL;
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL;

	mIsReliableUdpSending = FALSE;

	return TRUE;
}

BOOL CNetworkSession::End(void)
{
	CThreadSync sync; // ���� ������ ����ȭ

	if(!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);	// �ش� ��Ĺ�� Read/Write�� �����.
	closesocket(mSocket);		// ��Ĺ�� �ݴ´�.

	mSocket = NULL;

	if(mReliableUdpThreadHandle) {
		// ������ ���� �̺�Ʈ ����
		SetEvent(mReliableUdpThreadDestroyEvent);

		// ������ ����ñ��� ���Ѵ��
		// INFINITE�� �ƴ� 3~5�� ����� TerminateThread�� �̿��� ���� �����ص� ��
		WaitForSingleObject(mReliableUdpThreadHandle, INFINITE);
		CloseHandle(mReliableUdpThreadHandle);
	}

	if(mReliableUdpThreadDestroyEvent)
		CloseHandle(mReliableUdpThreadDestroyEvent);

	if(mReliableUdpThreadStartupEvent)
		CloseHandle(mReliableUdpThreadStartupEvent);

	if(mReliableUdpThreadWakeUpEvent)
		CloseHandle(mReliableUdpThreadWakeUpEvent);

	if(mReliableUdpWriteCompleteEvent)
		CloseHandle(mReliableUdpWriteCompleteEvent);

	// Reliable UDP�� ���� �����͸� �����ϴ� WriteQueue�� ����
	mReliableWriteQueue.End();

	return TRUE;
}


BOOL CNetworkSession::TcpBind(void)
{
	CThreadSync Sync; // ����ȭ ��ü

	if(mSocket)
		return FALSE;

	// TCP������ ��Ĺ�� ����
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(mSocket == INVALID_SOCKET)
		return FALSE;

	// TCP_NODELAY�ɼ�
	//BOOL noDelay = TRUE;
	//setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR*)&noDelay, sizeof(noDelay));

	return TRUE;
}


BOOL CNetworkSession::Listen(USHORT port, INT backLog)
{
	CThreadSync Sync;	// ����ȭ ��ü

	if(port <= 0 || backLog <= 0)
		return FALSE;

	if(!mSocket)
		return FALSE;

	// Listen�� ������ �����ϴ� ����
	SOCKADDR_IN listenSocketInfo;

	listenSocketInfo.sin_family = AF_INET;
	listenSocketInfo.sin_port = htons(port);
	listenSocketInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if(bind(mSocket, (struct sockaddr*)&listenSocketInfo, sizeof(SOCKADDR_IN) == SOCKET_ERROR) {
		End();

		return FALSE;
	}
	if(listen(mSocket, backLog) == SOCKET_ERROR) {
		End();

		return FALSE;
	}

	// LINGER ���� �ɼ� ó��
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger= 0;

	if(setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(linger)) == SOCKET_ERROR) {
		End();

		return FALSE;
	}
	
	return TRUE;
}

BOOL CNetworkSession::Connect(LPSTR address, USHORT port)
{
	CThreadSync sync;
	if(!address || port <= 0)
		return FALSE;

	if(!mSocket)
		return FALSE;

	SOCKADDR_IN remoteAddressInfo;

	remoteAddressInfo.sin_family = AF_INET;
	// ������ ��Ʈ�� htons�� ���� ��ȯ�ؼ� �־��ش�
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(address);
	// inet_addr�� �Ϲ� ���ڿ��� unsigned long���� ��ȯ���ش�.

	// ���� ������ �ּҷ� ���� �õ�
	// WSAConnect Ȥ�� ��Ŭ�� ��Ĺ �Լ��� connect�� �̿��Ҽ� �ִ�.
	if(WSAConnect(mSocket, (LPSOCKADDR) &remoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		//���������� �����¶�� ������ ����
		if(WSAGetLastError() != WSAEWOULDBLOCK) {
			End();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNetworkSession::Accept(SOCKET listenSocket)
{
	CThreadSync sync;

	if(!listenSocket)
		return FALSE;

	if(mSocket)
		return FALSE;

	// ACCEP�� ������ ��Ĺ ����
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == mSocket) {
		End();

		return FALSE;
	}

	//BOOL noDelay = TRUE;
	//setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR*)&noDelay, sizeof(noDelay));

	// AcceptEx��� (Accept�� ����ϸ� �ش� ������ ó���ϴ� ���� �ٸ� ó���� �Ҽ� ����
	// ���� ������ ������� ���� ���и� �Ҽ� �����Ƿ� �̸� ������ ��Ĺ�� ����Ͽ� Acceptó���� �Ѵ�)
	if(!AcceptEx(listenSocket, 
				mSocket, 
				mReadBuffer,			// recive buffer 
				0,						// ���� ����� 0���� �־� ���� ����
				sizeof(sockaddr_in) + 16, // ���� �ּ� (16����Ʈ Ŀ����)
				sizeof(sockaddr_in) + 16, // ����Ʈ �ּ� (16����Ʈ Ŀ����)
				NULL, 
				&mAcceptOverlapped.overLapped)) {
		if(WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNetworkSession::InitializeReadForIocp(void)
{
	CThreadSync sync;

	if(!mSocket)
		return FALSE;

	WSABUF wsaBuf;
	DWORD readBytes = 0;
	DWORD readFlag = 0;

	wsaBuf.buf = (CHAR*) mReadBuffer;
	wsaBuf.len = MAX_BUFFER_LENGTH;

	INT returnValue = WSARecv(mSocket,
								&wsaBuf,
								1, // ������ ���� �Է�,WsaBuf�� �̿��� ���� ���� ���� 1�̻��Է�
								&readBytes,
								&readFlag,
								&mReadOverlapped.overLapped,
								NULL);

	// SOCKET_ERROR������ WSA_IO_PENDING or WSAEWOULDBLOCK�̸� ��������
	if(SOCKET_ERROR == returnValue && WSA_IO_PENDING != WSAGetLastError() && WSAEWOULDBLOCK != WSAGetLastError()) {
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL CNetworkSession::ReadForIocp(BYTE* data, DWORD &dataLength)
{
	CThreadSync sync;

	if(!mSocket)
		return FALSE;

	if(!data || dataLength <= 0)
		return FALSE;

	// WSARecv�� ���� �޾ƿ� �����Ͱ� �ִ� mReadBuffer�� ���� ������ ����
	CopyMemory(data, mReadBuffer, dataLength);

	return TRUE;
}

BOOL CNetworkSession::ReadForEventSelect(BYTE* data, DWORD &dataLength)
{
	CThreadSync sync;

	if(!mSocket)
		return FALSE;

	if(!data)
		return FALSE;

	WSABUF wsaBuf;
	DWORD readBytes = 0;
	DWORD readFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer;
	wsaBuf.len = MAX_BUFFER_LENGTH;

	INT returnValue = WSARecv(mSocket,
								&wsaBuf,
								1,
								&readBytes,
								&readFlag,
								&mReadOverlapped.overLapped,
								NULL);

	if(SOCKET_ERROR == returnValue && WSA_IO_PEDDING != WSAGetLastError() && WSAEWOULDBLOCK != WSAGetLastError()) {
		End();

		return FALSE;
	}

	// ���� ���� ����
	CopyMemory(data, mReadBuffer, readBytes);
	// ���� ������ ���̵� �־��ش�.
	dataLength = readBytes;

	return TRUE;
}

BOOL CNetworkSession::Write(BYTE* data, DWORD dataLength) 
{
	CThreadSync sync;

	if(!mSocket)
		return FALSE;

	if(!data || 0 <= dataLength)
		return FALSE;

	WSABUF wsaBuf;			// write���� WSABUF�� ���
	DWORD writeBytes = 0;
	DWORD writeFlag = 0;

	wsaBuf.buf = (CHAR*)data;	// ������ ������ �����͸� �Է�
	wsaBuf.len = dataLength;

	INT returnValue = WSASend(mSocket, 
								&wsaBuf, 
								1, 
								&writeBytes, 
								writeFlag, 
								&mWriteOverlapped.overLapped, 
								NULL);

	if(SOCKET_ERROR == returnValue && WSA_IO_PENDING == WSAGetLastError() && WSAEWOULDBLOCK == WSAGetLastError()) {
		End();

		return FALSE;
	}

	return TRUE;
}


// UDP�� ����ϱ� ���� bind

BOOL CNetworkSession::UdpBind(USHORT port)
{
	CThreadSync sync;

	if(mSocket)
		return FALSE;

	// ����� UDP�ּ�, ��Ʈ�� ����
	SOCKADDR_IN remoteAddressInfo;

	remoteAddressInfo.sin_family = AF_INET;
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// UDP ��Ĺ�� ����
	mSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == mSocket)
		return FALSE;

	// ������ �ּҿ� bind
	if(SOCKET_ERROR == bind(mSocket, (struct sockaddr*)&remoteAddressInfo, sizeof(SOCKADDR_IN))) {
		End();
		
		return FALSE;
	}

	// ReliableUDPThread���� ����� �ڵ�� �̺�Ʈ ����
	mReliableUdpThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
	if(NULL == mReliableUdpThreadDestroyEvent) {
		End();

		return FALSE;
	}

	mReliableUdpThreadStartupEvent = CreateEvent(0, FALSE, FALSE, 0);
	if(NULL == mReliableUdpThreadStartupEvent) {
		End();

		return FALSE;
	}

	mReliableUdpThreadWakeUpEvent = CreateEvent(0, FALSE, FALSE, 0);
	if(NULL == mReliableUdpThreadWakeUpEvent) {
		End();

		return FALSE;
	}

	mReliableUdpWriteCompleteEvent = CreateEvent(0, FALSE, FALSE, 0);
	if(NULL == mReliableUdpWriteCompleteEvent) {
		End();

		return FALSE;
	}

	// ReliagleUDPThread���� ����� ť�� �ʱ�ȭ
	if(!mReliableWriteQueue.Begin()) {
		End();

		return FALSE;
	}

	// ReliableUdpThread����
	DWORD ReliableUdpThreadID = 0;
	mReliableUdpThreadHandle = CreateThread(NULL, 0, ::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);

	WaitForSingleObject(mReliableUdpThreadStartupEvent, INFINITE);

	return TRUE;
}

void CNetworkSession::ReliableUdpThreadCallback(void)
{
	DWORD eventID = 0;		// ���۰� ���Ḧ üũ�ϴ� �̺�Ʈ ID ��
	HANDLE threadEvents[2] = {mReliableUdpThreadDestroyEvent, mReliableUdpThreadWakeUpEvent};

	CHAR remoteAddress[32] = {0,};
	USHORT remotePort = 0;
	BYTE data[MAX_BUFFER_LENGTH] = {0,};
	DWORD dataLength = 0;
	VOID* object = NULL;

	while(TRUE) {
		// ReliableUdpThread������ �˸��� �̺�Ʈ UdpBind�� wait�� �����ش�.
		SetEvent(mReliableUdpThreadStartupEvent);

		// � �̺�Ʈ�� �߻��ϱ� ������ ���Ѵ�� ���
		eventID = WaitForMultipleObjects(2, threadEvents, FALSE, INFINITE);
		switch(eventID) {
		case WAIT_OBJECT_0:		// ����
			return;
		case WAIT_OBJECT_0+1:
			if(mReliableWriteQueue.Pop(&object, data, dataLength, remoteAddress, remotePort)) {
				// �����Ͱ� �ִ� ���
				// ���� Write�� ���ְ� WaitForSingleObject�� ���ش�.

				// ������ �����ϴ� �Լ�
				if(!WriteTo2(remoteAddress, remotePort, data, dataLength))
					return;

				// ������ ���⸦ 0.1�ʵ��� ��ٸ�
				DWORD result = WaitForSingleObject(mReliableUdpWriteCompleteEvent, 10);

				//�޾��� ��� ��� ó��
				if(WAIT_OBJECT_0 == result)
					goto NEXT_DATA;
				else
					goto RETRY;
			}
			else
				mIsReliableUdpSending == FALSE;
			
			break;

		}
	}
}