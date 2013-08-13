#pragma once
class CIocp
{
public:
	CIocp(void);
	virtual ~CIocp(void);

	BOOL Begin(void);
	BOOL End(void);
	BOOL RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey);
	void WorkerThreadCallback(void);

	virtual void OnIOConnected(void*) = 0;
	virtual void OnIODisconnected(void*) = 0;
	virtual void OnIORead(void* object, int num) = 0;
	virtual void OnIOWrite(void* object, int num) = 0;

private:
	HANDLE mIocpHandle;
	HANDLE mStartupEventHandle;
	DWORD mWorkerThreadCount;

	std::vector<HANDLE> mWorkerThreadVector;

};

