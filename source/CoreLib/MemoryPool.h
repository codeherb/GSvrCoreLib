#pragma once

template <class T, int ALLOC_BLOCK_SIZE = 50>
class CMemoryPool : public CMultiThreadSync<T>
{
public:
	
	static VOID* operator new(std::size_t allocLength)
	{
		CThreadSync Sync;	//동기화 객체

		assert(sizeof(T) == allockLength);
		assert(sizeof(T) >= sizeof(UCHAR*));

		// 만약 더 이상 할당할 수 있는 공간이 없을 경우 새로 할당
		if(!mFreePointer)
			allocBlock();
		UCHAR* ReturnPointer = mFreePointer;
		mFreePointer = *reinterpret_cast<UCHAR**>(ReturnPointer);
		// mFreePointer에는 리턴하는 블록 앞에 4바이트 주소가 들어간다.

		return ReturnPointer;
	}

	static VOID operator delete(VOID* deletePointer)
	{
		CThreadSync Sync;	//동기화 객체

		*reinterpret_cast<UCHAR**>(deletePointer) = mFreePointer;
		// delete 된 블록의 next에 현재 mFreePointer의 주소를 넣어준다.
		mFreePointer = static_cast<UCHAR*>(deletePointer);
		// mFreePointer를 갱신
	}

private:
	/**
	 * Linked List 형태의 메모리 블록 생성 
	 * NextPointer(4바이트), 데이터메모리 영역
	 */
	static void allocBlock()
	{
		mFreePointer = new UCHAR[sizeof(T) * ALLOCK_BLOCK_SIZE];
		// mFreePointer에 미리 메모리를 할당하여 연결

		UCHAR **Current = reinterpret_cast<UCHAR**>(mFreePointer);
		// 할당한 메모리의 첫 블록의 포인터를 Current에 저장
		// '연관성 없는 포인터 타입'의 무조건적인 캐스팅을 위해 reinterpret_cast사용
		UCHAR* Next = mFreePointer; // 할당된 메모리 첫 블록

		for(INT i = 0; i < ALLOCK_BLOCK_SIZE - 1; ++i)
		{
			Next += sizeof(T);
			*Current = Next;	// 할당된 메모리 앞 4바이트에 다음 블록의 주소 저장
			Current = reinterpret_cast<UCHAR**>(Next);
		}

		*Current = 0;
	}

private:
	static UCHAR* mFreePointer;

protected:
	//CMemoryPool(void)
	//{
	//}

	~CMemoryPool(void)
	{
	}
};

template <class T, int ALLOCK_BLOCK_SIZE>
UCHAR* CMemoryPool<T, ALLOCK_BLOCK_SIZE>::mFreePointer;