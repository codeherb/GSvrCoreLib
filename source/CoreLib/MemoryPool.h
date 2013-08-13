#pragma once

template <class T, int ALLOC_BLOCK_SIZE = 50>
class CMemoryPool : public CMultiThreadSync<T>
{
public:
	
	static VOID* operator new(std::size_t allocLength)
	{
		CThreadSync Sync;	//����ȭ ��ü

		assert(sizeof(T) == allockLength);
		assert(sizeof(T) >= sizeof(UCHAR*));

		// ���� �� �̻� �Ҵ��� �� �ִ� ������ ���� ��� ���� �Ҵ�
		if(!mFreePointer)
			allocBlock();
		UCHAR* ReturnPointer = mFreePointer;
		mFreePointer = *reinterpret_cast<UCHAR**>(ReturnPointer);
		// mFreePointer���� �����ϴ� ��� �տ� 4����Ʈ �ּҰ� ����.

		return ReturnPointer;
	}

	static VOID operator delete(VOID* deletePointer)
	{
		CThreadSync Sync;	//����ȭ ��ü

		*reinterpret_cast<UCHAR**>(deletePointer) = mFreePointer;
		// delete �� ����� next�� ���� mFreePointer�� �ּҸ� �־��ش�.
		mFreePointer = static_cast<UCHAR*>(deletePointer);
		// mFreePointer�� ����
	}

private:
	/**
	 * Linked List ������ �޸� ��� ���� 
	 * NextPointer(4����Ʈ), �����͸޸� ����
	 */
	static void allocBlock()
	{
		mFreePointer = new UCHAR[sizeof(T) * ALLOCK_BLOCK_SIZE];
		// mFreePointer�� �̸� �޸𸮸� �Ҵ��Ͽ� ����

		UCHAR **Current = reinterpret_cast<UCHAR**>(mFreePointer);
		// �Ҵ��� �޸��� ù ����� �����͸� Current�� ����
		// '������ ���� ������ Ÿ��'�� ���������� ĳ������ ���� reinterpret_cast���
		UCHAR* Next = mFreePointer; // �Ҵ�� �޸� ù ���

		for(INT i = 0; i < ALLOCK_BLOCK_SIZE - 1; ++i)
		{
			Next += sizeof(T);
			*Current = Next;	// �Ҵ�� �޸� �� 4����Ʈ�� ���� ����� �ּ� ����
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