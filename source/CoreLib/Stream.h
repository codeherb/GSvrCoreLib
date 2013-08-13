#pragma once

class CStream
{
public:
	CStream(void);
	virtual ~CStream(void);

	BOOL SetBuffer(BYTE* buffer);

	BOOL ReadInt32(INT* data);
	BOOL ReadDWORD(DWORD* data); // 4byte
	BOOL ReadDWORD_PTR(DWORD_PTR* data);
	BOOL ReadByte(BYTE* data);
	BOOL ReadBytes(BYTE* data, DWORD length);
	BOOL ReadFloat(FLOAT* data);
	BOOL ReadInt64(INT64* data);
	BOOL ReadUSHORT(USHORT* data);
	BOOL ReadSHORT(SHORT* data);
	BOOL ReadBOOL(BOOL* data);

	BOOL WriteInt32(const INT& data);
	BOOL WriteDWORD(const DWORD& data);
	BOOL WriteDWORD_PTR(const DWORD_PTR& data);
	BOOL WriteByte(const BYTE& data);
	BOOL WriteBytes(BYTE* data, const DWORD& length);
	BOOL WriteFloat(const FLOAT& data);
	BOOL WriteInt64(const INT64& data);
	BOOL WriteUSHORT(const USHORT& data);
	BOOL WriteSHORT(const SHORT& data);
	BOOL WriteBOOL(const BOOL& data);

	DWORD GetLength(VOID);
private:
	BYTE* mBufferPointer;
	int	mLength;
};

class CStreamSP {
public:
	CStreamSP() {
		mStream = new CStream();
	};

	~CStreamSP(){
		delete mStream;
	};

	CStream* operator->(VOID) {
		return mStream;
	}
	operator CStream*(VOID) {
		return mStream;
	}
private:
	CStream* mStream;
};
