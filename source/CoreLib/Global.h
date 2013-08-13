#pragma once

#include <iostream>
#include <tchar.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <dbghelp.h>
#include <assert.h>
#include <time.h>

#include <vector>

enum IO_TYPE {
	IO_READ,
	IO_WRITE,
	IO_ACCEPT
};

typedef struct _OVERLAPPED_EX
{
	OVERLAPPED overLapped;
	IO_TYPE ioType;
	VOID *object;
} OVERLAPPED_EX;

#define MAX_BUFFER_LENGTH 4096

#ifdef _SERVER_SIDE
#define MAX_QUEUE_LENGTH 50
#else
#define MAX_QUEUE_LENGTH 500
#endif