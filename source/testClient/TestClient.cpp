// testClient.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

