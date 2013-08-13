#pragma once

class CRandom
{
public:
	CRandom(void);
	virtual ~CRandom(void);

private:
	UINT mSeed;

public:
	BOOL Init(UINT seed);

	INT Rand(VOID);
	INT SimpleRandom(VOID);
	INT ComplexRandom(VOID);
};

