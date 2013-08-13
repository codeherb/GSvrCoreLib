#include "stdafx.h"
#include "Stream.h"

CStream::CStream(void)
{
}


CStream::~CStream(void)
{
}

BOOL CStream::SetBuffer(BYTE* buffer) {
	if(!buffer)
		return FALSE;

	mBufferPointer = buffer;
	mLength = 0;


	return TRUE;
}

BOOL CStream::ReadInt32(INT* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

BOOL CStream::ReadDWORD(DWORD* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

BOOL CStream::ReadDWORD_PTR(DWORD_PTR* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL CStream::ReadByte(BYTE* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

BOOL CStream::ReadBytes(BYTE* data, DWORD length) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, length);

	mLength += length;

	return TRUE;
}

BOOL CStream::ReadFloat(FLOAT* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

BOOL CStream::ReadInt64(INT64* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

BOOL CStream::ReadSHORT(SHORT* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

BOOL CStream::ReadUSHORT(USHORT* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

BOOL CStream::ReadBOOL(BOOL* data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(data, mBufferPointer + mLength, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL CStream::WriteInt32(const INT& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

BOOL CStream::WriteDWORD(const DWORD& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

BOOL CStream::WriteDWORD_PTR(const DWORD_PTR& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL CStream::WriteByte(const BYTE& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

BOOL CStream::WriteBytes(BYTE* data, const DWORD& length) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, data, length);

	mLength += length;

	return TRUE;
}

BOOL CStream::WriteFloat(const FLOAT& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

BOOL CStream::WriteInt64(const INT64& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

BOOL CStream::WriteSHORT(const SHORT& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

BOOL CStream::WriteUSHORT(const USHORT& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

BOOL CStream::WriteBOOL(const BOOL& data) {
	// 값을 읽어 Data 변수에 저장
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

// 전체 길이를 받아올 때 사용
DWORD CStream::GetLength(VOID) {
	return mLength;
}