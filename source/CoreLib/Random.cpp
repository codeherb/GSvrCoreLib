#include "stdafx.h"
#include "Random.h"


CRandom::CRandom(void)
{
}


CRandom::~CRandom(void)
{
}

// 시드 값을 설정
BOOL CRandom::Init(UINT seed)
{
	mSeed = seed;

	return TRUE;
}

// 가장 일반적인 형태
INT CRandom::Rand(void)
{
	return (((mSeed = mSeed * 214013L + 2531011L) >> 16) & 0x7FFF);
}

// 간단한 계산으로 최소한의 랜덤값 생성
INT CRandom::SimpleRandom(void)
{
	return (((mSeed = mSeed + 2531011L) >> 3) & 0x7FFF);
}

// 복잡한 계산식을 통해 랜덤값 생성
INT CRandom::ComplexRandom(VOID)
{

	UINT next = mSeed;
	INT  result;

	next *= 1103515245;
	next += 12345;
	result = (UINT) (next / 65536) % 2048;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (UINT) (next / 65536) % 1024;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (UINT) (next / 65536) % 1024;

	mSeed = next;
	return result;
}