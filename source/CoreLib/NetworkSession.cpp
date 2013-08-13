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
	/* Overlapped구조체는 IO 리퀘스트에 대한 상태값을 저장하는 자료구조이다.
	 * 정의한 OVERLAPPED_EX는 이것이 어떤 일을 하는 변수인지 명확하게 알기 위한
	 * 사용자 정의 구조체로 내부에 Overlapped 변수를 가지고 있다. */

	// Accept 관련 Overlapped구조체
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	// Read 관련 Overlapped구조체
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	// Write 관련 Overlapped구조체
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	// 패킷을 받았다는 신호가 오기 전까지 동일 패킷을 계속전송해줄 스래드 핸들
	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;	// 스레드 시작 이벤트
	mReliableUdpThreadDestroyEvent = NULL;	// 스레드 종료 이벤트
	mReliableUdpThreadWakeUpEvent = NULL;	// 스레드 재시작 이벤트
	mReliableUdpWriteCompleteEvent = NULL;	// 패킷을 더이상 보낼 필요가 없을때 날리는 이벤트

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
	CThreadSync sync; // 다중 스레드 동기화

	if(mSocket) // 실행중인 소켓이 있으면 실패
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
	CThreadSync sync; // 다중 스레드 동기화

	if(!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);	// 해당 소캣의 Read/Write를 멈춘다.
	closesocket(mSocket);		// 소캣을 닫는다.

	mSocket = NULL;

	if(mReliableUdpThreadHandle) {
		// 스레드 종료 이벤트 생성
		SetEvent(mReliableUdpThreadDestroyEvent);

		// 스레드 종료시까지 무한대기
		// INFINITE가 아닌 3~5초 대기후 TerminateThread를 이용해 강제 종료해도 됨
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

	// Reliable UDP로 보낼 데이터를 저장하는 WriteQueue를 비운다
	mReliableWriteQueue.End();

	return TRUE;
}


BOOL CNetworkSession::TcpBind(void)
{
	CThreadSync Sync; // 동기화 개체

	if(mSocket)
		return FALSE;

	// TCP형태의 소캣을 생성
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(mSocket == INVALID_SOCKET)
		return FALSE;

	// TCP_NODELAY옵션
	//BOOL noDelay = TRUE;
	//setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR*)&noDelay, sizeof(noDelay));

	return TRUE;
}


BOOL CNetworkSession::Listen(USHORT port, INT backLog)
{
	CThreadSync Sync;	// 동기화 개체

	if(port <= 0 || backLog <= 0)
		return FALSE;

	if(!mSocket)
		return FALSE;

	// Listen할 정보를 저장하는 변수
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

	// LINGER 관련 옵션 처리
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
	// 접속할 포트를 htons를 통해 변환해서 넣어준다
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(address);
	// inet_addr은 일반 문자열을 unsigned long으로 변환해준다.

	// 위에 설정된 주소로 접속 시도
	// WSAConnect 혹은 버클리 소캣 함수인 connect를 이용할수 있다.
	if(WSAConnect(mSocket, (LPSOCKADDR) &remoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		//실패이지만 대기상태라고 나오면 성공
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

	// ACCEP시 연결할 소캣 생성
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == mSocket) {
		End();

		return FALSE;
	}

	//BOOL noDelay = TRUE;
	//setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR*)&noDelay, sizeof(noDelay));

	// AcceptEx사용 (Accept를 사용하면 해당 접속을 처리하는 동안 다른 처리를 할수 없다
	// 따라서 접속이 몰릴경우 접속 실패를 할수 있으므로 미리 생성된 소캣을 사용하여 Accept처리를 한다)
	if(!AcceptEx(listenSocket, 
				mSocket, 
				mReadBuffer,			// recive buffer 
				0,						// 버퍼 사이즈를 0으로 주어 받지 않음
				sizeof(sockaddr_in) + 16, // 로컬 주소 (16바이트 커야함)
				sizeof(sockaddr_in) + 16, // 리모트 주소 (16바이트 커야함)
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
								1, // 버퍼의 갯수 입력,WsaBuf를 이용한 원형 버퍼 사용시 1이상입력
								&readBytes,
								&readFlag,
								&mReadOverlapped.overLapped,
								NULL);

	// SOCKET_ERROR이지만 WSA_IO_PENDING or WSAEWOULDBLOCK이면 정상진행
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

	// WSARecv를 통해 받아온 데이터가 있는 mReadBuffer로 부터 데이터 복사
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

	// 받은 내용 복사
	CopyMemory(data, mReadBuffer, readBytes);
	// 받은 데이터 길이도 넣어준다.
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

	WSABUF wsaBuf;			// write에도 WSABUF를 사용
	DWORD writeBytes = 0;
	DWORD writeFlag = 0;

	wsaBuf.buf = (CHAR*)data;	// 보내는 데이터 포인터를 입력
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


// UDP를 사용하기 위한 bind

BOOL CNetworkSession::UdpBind(USHORT port)
{
	CThreadSync sync;

	if(mSocket)
		return FALSE;

	// 사용할 UDP주소, 포트를 실행
	SOCKADDR_IN remoteAddressInfo;

	remoteAddressInfo.sin_family = AF_INET;
	remoteAddressInfo.sin_port = htons(port);
	remoteAddressInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// UDP 소캣을 생성
	mSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == mSocket)
		return FALSE;

	// 설정된 주소에 bind
	if(SOCKET_ERROR == bind(mSocket, (struct sockaddr*)&remoteAddressInfo, sizeof(SOCKADDR_IN))) {
		End();
		
		return FALSE;
	}

	// ReliableUDPThread에서 사용할 핸들과 이벤트 생성
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

	// ReliagleUDPThread에서 사용할 큐를 초기화
	if(!mReliableWriteQueue.Begin()) {
		End();

		return FALSE;
	}

	// ReliableUdpThread생성
	DWORD ReliableUdpThreadID = 0;
	mReliableUdpThreadHandle = CreateThread(NULL, 0, ::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);

	WaitForSingleObject(mReliableUdpThreadStartupEvent, INFINITE);

	return TRUE;
}

void CNetworkSession::ReliableUdpThreadCallback(void)
{
	DWORD eventID = 0;		// 시작과 종료를 체크하는 이벤트 ID 값
	HANDLE threadEvents[2] = {mReliableUdpThreadDestroyEvent, mReliableUdpThreadWakeUpEvent};

	CHAR remoteAddress[32] = {0,};
	USHORT remotePort = 0;
	BYTE data[MAX_BUFFER_LENGTH] = {0,};
	DWORD dataLength = 0;
	VOID* object = NULL;

	while(TRUE) {
		// ReliableUdpThread시작을 알리는 이벤트 UdpBind의 wait를 끝내준다.
		SetEvent(mReliableUdpThreadStartupEvent);

		// 어떤 이벤트가 발생하기 전까지 무한대로 대기
		eventID = WaitForMultipleObjects(2, threadEvents, FALSE, INFINITE);
		switch(eventID) {
		case WAIT_OBJECT_0:		// 종료
			return;
		case WAIT_OBJECT_0+1:
			if(mReliableWriteQueue.Pop(&object, data, dataLength, remoteAddress, remotePort)) {
				// 데이터가 있는 경우
				// 실제 Write를 해주고 WaitForSingleObject를 해준다.

				// 데이터 전송하는 함수
				if(!WriteTo2(remoteAddress, remotePort, data, dataLength))
					return;

				// 응답이 오기를 0.1초동안 기다림
				DWORD result = WaitForSingleObject(mReliableUdpWriteCompleteEvent, 10);

				//받았을 경우 당므 처리
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