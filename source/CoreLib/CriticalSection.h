#pragma once

/**
 * window 내에서 스레드간 동기화를 위해 크리티컬섹션과 뮤텍스를 제공한다.
 * 크리티컬 섹션은 싱글 프로세스상에서 단순히 객체 이용 권한의 잠금과 풀림을
 * 통해 동기화 하여 빠른 처리가 가능해 많이 사용하며 
 * 뮤텍스는 OS 단을 통해 프로세스 동기화 처리까지 하도록 되어있어 다소 느리다.
 * 하지만 리눅스에서는 크리티컬 섹션이 뮤텍스를 이용해 구현되있어 속도 차이가 거의 없다.
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