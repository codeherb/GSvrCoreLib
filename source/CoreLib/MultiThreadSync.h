#pragma once

template <class T>
class CMultiThreadSync
{
	friend class CThreadSync;
public:
	class CThreadSync
	{
	public:
		CThreadSync(void) {
			T::mSync.Enter();
		}

		~CThreadSync(void) {
			T::mSync.Leave();
		}
	};

private:
	static CCriticalSection mSync; // CRITICAL_SECTION °´Ã¼
};

template <class T>
CCriticalSection CMultiThreadSync<T>::mSync;