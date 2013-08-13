#pragma once

/**
 * window ������ �����尣 ����ȭ�� ���� ũ��Ƽ�ü��ǰ� ���ؽ��� �����Ѵ�.
 * ũ��Ƽ�� ������ �̱� ���μ����󿡼� �ܼ��� ��ü �̿� ������ ��ݰ� Ǯ����
 * ���� ����ȭ �Ͽ� ���� ó���� ������ ���� ����ϸ� 
 * ���ؽ��� OS ���� ���� ���μ��� ����ȭ ó������ �ϵ��� �Ǿ��־� �ټ� ������.
 * ������ ������������ ũ��Ƽ�� ������ ���ؽ��� �̿��� �������־� �ӵ� ���̰� ���� ����.
 */
class CCriticalSection
{
public:
	CCriticalSection(void) {
		InitializeCriticalSection(&mSync);
	}
	~CCriticalSection(void) {
		DeleteCriticalSection(&mSync);
	}

	inline void Enter(void) {
		EnterCriticalSection(&mSync);
	}
	inline void Leave(void) {
		LeaveCriticalSection(&mSync);
	}
private:
	CRITICAL_SECTION mSync;
};