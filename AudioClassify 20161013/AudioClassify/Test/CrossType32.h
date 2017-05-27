/*
 *	Define cross data and function type
 *
 *	Copyright	(c)	Speech Processing Laboratory of hit
 *	Designed by SPL
 *	Our address: Speech Processing Laboratory, heilongjiang province, china
 *	
 *	New data types is defined to replace the windows routine types
 */


 #ifndef	_STDINT_H
 #ifndef	CROSSTYPE_H

 #define	CROSSTYPE_H

 typedef	unsigned short	uint16_t;
 typedef	unsigned char	uint8_t;
 typedef	char			int8_t;
 typedef	unsigned int	uint32_t;
 typedef	short			int16_t;
 typedef	int				int32_t;
 
 #endif
 #endif
 
#ifdef _WIN32
 typedef	__int64 int64_t;
#endif

 #ifdef	__GNUG__
 #include	<pthread.h>
 #include	<netinet/in.h> 
 #include	<sys/types.h>
 #include	<sys/socket.h>

 typedef	long long		__int64;
 typedef	int				__int32;
 typedef	bool				BOOL;
 typedef	long long		int64_t;
 typedef	unsigned long long	uint64_t;

 typedef	void (*AFX_THREADPROC)(void *);
 typedef	pthread_t *		HANDLE;
 typedef    pthread_t * HWND;
 
 typedef	void *			LPVOID;
 typedef	unsigned int			SOCKET;
 typedef	struct sockaddr_in	SOCKADDR_IN;
 typedef	struct sockaddr_in	sockaddr_in;
 typedef	struct sockaddr		SOCKADDR; 

 #ifndef	TRUE
 #define	TRUE 	1
 #endif

 #ifndef	FALSE
 #define	FALSE	0
 #endif

 #ifndef	WINAPI
 #define	WINAPI
 #endif

 #ifndef	CALLBACK
 #define	CALLBACK
 #endif

 #ifndef	PASCAL
 #define	PASCAL
 #endif

 #ifndef	EXPORT
 #define	EXPORT
 #endif

 #ifndef	__stdcall
 #define	__stdcall
 #endif

 #ifndef	SOCKET_ERROR
 #define	SOCKET_ERROR	-1
 #endif

 #ifndef	INVALID_SOCKET
 #define	INVALID_SOCKET	-1
 #endif

#ifndef		TCP_NODELAY
#define		TCP_NODELAY	0
#endif

#ifndef		SO_DONTLINGER
#define		SO_DONTLINGER	0
#endif
 
 
 #endif
