// testClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "StdAfx.h"
#include "include/TestClient.h"


/************************************************************************/
/* Test function to be called in main */
/************************************************************************/
int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv); //initialize GTest

	RUN_ALL_TESTS();

	std::getchar();

	return 0;
}

