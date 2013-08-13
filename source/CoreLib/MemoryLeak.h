#pragma once

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC	// VC++이고 Debug모드인 경우 선언
#include <crtdbg.h>
#ifndef _CONSOLE
#include <cstdlib>
#endif

class CMemoryLeak
{
public:

	CMemoryLeak(void)
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#ifdef _CONSOLE // 콘솔인 경우 디버깅 셋팅
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#define DEBUG_NORMALBLOCK new(_NORMAL_BLOCK, __FILE__, __LINE__)
#ifdef new
#undef new
#endif
#define new DEBUG_NORMALBLOCK

#else	// 기타 모드 인 경우
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

#ifdef malloc
#undef malloc
#endif
#define malloc(s) (_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__))
	}

};

static CMemoryLeak MemoryLeak;

#endif
#endif

