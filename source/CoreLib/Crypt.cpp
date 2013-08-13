#include "stdafx.h"
#include "Crypt.h"

const INT C1 = 52845;
const INT C2 = 22719;
const INT KEY = 72957;

BOOL CCrypt::Encrypt(BYTE* source, BYTE* destination, DWORD length)
{
	DWORD i;
	INT Key = KEY;

	if(!source || !destination || 0 >= length)
		return FALSE;

	// 바이트 단위로 암호화
	for(i = 0; i < length; i++)
	{
		destination[i] = source[i]^Key >> 8;
		// 1바이트를 Key값 만큼 제곱을 해주고 8비트 밀어준다.
		Key = (destination[i] + Key) * C1 +C2;
		// 다음 바이트에서 사용할 키를 C1, C2를 이용해 변경해준다.
	}

	return TRUE;
}


BOOL CCrypt::Decrypt(BYTE* source, BYTE* destination, DWORD length)
{
	DWORD i;
	BYTE PreviousBlock;
	INT Key = KEY;

	if(!source || !destination || 0 >= length)
		return FALSE;

	// 바이트 단위로 복호화
	for(i = 0; i < length; i++)
	{
		PreviousBlock = source[i];
		destination[i] = source[i]^Key >> 8;
		Key = (PreviousBlock + Key) * C1 + C2;
		// 이전 블록을 저장해 두었다 복호화에 사용되는 키를 알아낸다.
	}

	return TRUE;
}
