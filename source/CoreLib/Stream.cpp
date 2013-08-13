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
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

BOOL CStream::ReadDWORD(DWORD* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

BOOL CStream::ReadDWORD_PTR(DWORD_PTR* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL CStream::ReadByte(BYTE* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

BOOL CStream::ReadBytes(BYTE* data, DWORD length) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, length);

	mLength += length;

	return TRUE;
}

BOOL CStream::ReadFloat(FLOAT* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

BOOL CStream::ReadInt64(INT64* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

BOOL CStream::ReadSHORT(SHORT* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

BOOL CStream::ReadUSHORT(USHORT* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

BOOL CStream::ReadBOOL(BOOL* data) {
	// ���� �о� Data ������ ����
	CopyMemory(data, mBufferPointer + mLength, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL CStream::WriteInt32(const INT& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

BOOL CStream::WriteDWORD(const DWORD& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

BOOL CStream::WriteDWORD_PTR(const DWORD_PTR& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL CStream::WriteByte(const BYTE& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

BOOL CStream::WriteBytes(BYTE* data, const DWORD& length) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, data, length);

	mLength += length;

	return TRUE;
}

BOOL CStream::WriteFloat(const FLOAT& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

BOOL CStream::WriteInt64(const INT64& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

BOOL CStream::WriteSHORT(const SHORT& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

BOOL CStream::WriteUSHORT(const USHORT& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

BOOL CStream::WriteBOOL(const BOOL& data) {
	// ���� �о� Data ������ ����
	CopyMemory(mBufferPointer + mLength, &data, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

// ��ü ���̸� �޾ƿ� �� ���
DWORD CStream::GetLength(VOID) {
	return mLength;
}