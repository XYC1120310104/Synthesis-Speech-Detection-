/*
 * Define cross data and function type
 *
 * Copyright (c) Speech Processing Laboratory of hit
 * Designed by SPL
 * Our address: Speech Processing Laboratory, heilongjiang province, china
 * 
 * New data types is defined to replace the windows routine types
 */
#ifdef _WIN32
 typedef	__int64           int64_t;
 typedef	unsigned __int64 uint64_t;
#endif

#ifdef __GNUG__

#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef CROSSTYPE_H
#define CROSSTYPE_H

#ifndef _STDINT_H
	typedef unsigned short uint16_t;
	typedef unsigned char uint8_t;
	typedef unsigned int uint32_t;
	typedef short    int16_t;
	typedef int      int32_t;
	typedef long long  __int64;
#endif


#ifndef HWND
struct PHWND__{int unused;};
typedef struct PHWND__ *HWND;
#endif

typedef int    _int32;
typedef short    _int16;
typedef char    _int8;
//  typedef long int _int64;


typedef unsigned int   UINT;
typedef int    INT;
typedef unsigned int   DWORD;
typedef short    WORD;
typedef unsigned char   BYTE;
typedef bool    BOOL;
typedef void (*AFX_THREADPROC)(void *);
typedef pthread_t *  HANDLE;
typedef void *   LPVOID;
typedef unsigned int   SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr  SOCKADDR;

#pragma pack(push)
#pragma pack(1)

struct tWAVEFORMATEX
{
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
  uint16_t cbSize;
};
typedef struct tWAVEFORMATEX WAVEFORMATEX;
typedef struct tWAVEFORMATEX *PWAVEFORMATEX;
#pragma pack(pop)


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef WINAPI
#define WINAPI
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

#ifndef PASCAL
#define PASCAL
#endif

#ifndef EXPORT
#define EXPORT
#endif

#ifndef __stdcall
#define __stdcall
#endif

#ifndef SOCKET_ERROR
 #define SOCKET_ERROR -1
 #endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef  TCP_NODELAY
#define  TCP_NODELAY 0
#endif

#ifndef  SO_DONTLINGER
#define  SO_DONTLINGER 0
#endif

#endif
#endif
