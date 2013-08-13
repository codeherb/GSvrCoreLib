#pragma once

class CNetworkSession : public CMultiThreadSync<CNetworkSession>
{
public:
	CNetworkSession(void);
	virtual ~CNetworkSession(void);

	BOOL Begin(void);
	BOOL End(void);

	BOOL TcpBind(void);
	BOOL Listen(USHORT port, INT backLog);
	BOOL Accept(SOCKET listenSocket);
	BOOL Connect(LPSTR address, USHORT port);
	BOOL Write(BYTE* data, DWORD dataLength);

	//For IOCP
	BOOL InitializeReadForIocp(void);
	BOOL ReadForIocp(BYTE* data, DWORD &dataLength);

	//For EventSelect
	BOOL ReadForEventSelect(BYTE* data, DWORD &dataLength);

	// UDP
	BOOL UdpBind(USHORT port);
	void ReliableUdpThreadCallback(void);
private:
	OVERLAPPED_EX mAcceptOverlapped;
	OVERLAPPED_EX mReadOverlapped;
	OVERLAPPED_EX mWriteOverlapped;

	BYTE mReadBuffer[MAX_BUFFER_LENGTH];
	SOCKADDR_IN		mUdpRemoteInfo;

	SOCKET			mSocket;

	HANDLE			mReliableUdpThreadHandle;
	HANDLE			mReliableUdpThreadStartupEvent;
	HANDLE			mReliableUdpThreadDestroyEvent;
	HANDLE			mReliableUdpThreadWakeUpEvent;
	HANDLE			mReliableUdpWriteCompleteEvent;

	CCircularQueue	mReliableWriteQueue;

	BOOL			mIsReliableUdpSending;
};

