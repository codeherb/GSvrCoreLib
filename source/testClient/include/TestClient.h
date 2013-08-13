#pragma once


namespace {

	class StreamTest : public ::testing::Test {
	protected:
		virtual void SetUp() {
			buffer = new BYTE[1024];
			mStreamWriter.SetBuffer(buffer);
		}
		virtual void TearDown() {
		}

		BYTE* buffer;
		CStream mStreamWriter;
		CStream mStreamReader;
	};

	/***********************************************/
	/* We write test case here */
	/***********************************************/
	BYTE G_RW_TARGET = 0xF1;
	TEST_F(StreamTest, testByteStreamRW)
	{
		// Given
		//BYTE* buffer = new BYTE[1024];
		//CStream stream;
		//stream.SetBuffer(buffer);

		// When
		BYTE result;
		BOOL bResult = false;
		mStreamWriter.WriteByte(G_RW_TARGET);
		mStreamReader.SetBuffer(buffer);
		bResult = mStreamReader.ReadByte(&result);

		// Then
		ASSERT_TRUE(bResult);
		ASSERT_EQ(result, G_RW_TARGET);
	}

	TEST(CRegistryTest, testRegistryObj) 
	{
		// Given
		CRegistry registry;

		// when
		// HKEY_LOCAL_MACHINE에서 아래의 서브키를 가지는 서브키를 연다.
		BOOL result = registry.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib"));

		// then
		ASSERT_TRUE(result);
	}

	TEST(CIniFile, testIniFileObj)
	{
		// Given
		CIniFile iniFile;
		BOOL result;
		
		// When
		result = iniFile.Open(_T("C:\\TestInit.ini"));

		// Then
		ASSERT_TRUE(result);

		// When
		result = iniFile.SetValue(_T("SERVER CONFIG"), _T("PORT"), (DWORD)8080);

		// Then
		ASSERT_TRUE(result);

		// When
		result = iniFile.Close();

		// Then
		ASSERT_TRUE(result);
	}

	typedef struct _QUEUE_DATA
	{
		INT iData1;
		BYTE aData2[100];
	} QUEUE_DATA;

	TEST(CircularQueue, testCircularQueue)
	{
		/*CCircularQueue<QUEUE_DATA> queue;
		queue.Begin();

		// 입력할 데이터 정의
		QUEUE_DATA pushData;
		pushData.iData1 = 10;
		ZeroMemory(pushData.aData2, sizeof(pushData.aData2));
		queue.Push(pushData);

		// 출력받을 데이터 정의
		QUEUE_DATA popData;
		queue.Pop(popData);*/

	}

	typedef struct _data {
		INT ACTION_SLOT_POSITION;
		INT TARGET_SLOT_POSITION;
		INT ACTION_TYPE;
		INT PARAMETER;
	} ActionData;

	TEST(Loger, testLoger)
	{
		ActionData Data;
		CLog::WriteLog(_T("RAW PACKET DATA : ACTION_SLOT_POSITION(%d), TARGET_SLOT_POSITION(%d), ACTION_TYPE(%d), PARAMETER(%d)"),
			Data.ACTION_SLOT_POSITION,
			Data.TARGET_SLOT_POSITION,
			Data.ACTION_TYPE,
			Data.PARAMETER);
	}

	TEST(Crypter, testCrypt)
	{
		BYTE* src = reinterpret_cast<BYTE*> (_T("테스트 소스 문자열"));
		BYTE* encrypted = new BYTE[sizeof(*src)];
		BYTE* decrypted = new BYTE[sizeof(*src)];
		ASSERT_TRUE(CCrypt::Encrypt(src, encrypted, sizeof(*src)));
		CCrypt::Decrypt(encrypted, decrypted, sizeof(*src));
		ASSERT_EQ(*src, *decrypted);
	}

	TEST(MemoryLeakChecker, testMemoryLeakChecker)
	{
		BYTE *pData = new BYTE[1024];

		ZeroMemory(pData, 1024 * sizeof(BYTE));
		
	}
} // end namespace