#ifndef _TYPEDEF_H__
#define _TYPEDEF_H__

#ifndef WIN32

typedef unsigned char       UINT8;
typedef char                INT8;
typedef unsigned short      UINT16;
typedef short               INT16;
typedef unsigned int 		UINT32;
typedef int                 INT32;
typedef unsigned long long  UINT64;
typedef long long 			INT64;

#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWWORD(l)          ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIGHWORD(l)         ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOWBYTE(w)          ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIGHBYTE(w)         ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

typedef unsigned long   DWORD;
typedef unsigned long   ULONG;
typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned short	WORD;
typedef void*           HANDLE;
typedef unsigned int    UINT;
typedef void*			PVOID;
typedef unsigned char   BOOLEAN;
typedef void			VOID;
typedef	unsigned int*	PULONG;
typedef UCHAR* PUCHAR;
typedef long			LONG;
typedef DWORD ADAPTER_HANDLE;
typedef char			CHAR;
typedef unsigned char	BYTE;
typedef unsigned long   LRESULT;
typedef long HRESULT;
typedef char TCHAR;
typedef unsigned char _TUCHAR;
typedef const TCHAR* LPCTSTR;
typedef TCHAR* LPTSTR;
typedef CHAR* LPSTR;
typedef const CHAR* LPCSTR;
typedef int SOCKET;
typedef unsigned long	netip_t;		//网络字节序的ip
typedef unsigned long	hostip_t;		//主机字节序的ip
typedef unsigned short	netport_t;		//网络字节序的port
typedef unsigned short	hostport_t;		//主机字节序的port
typedef unsigned char	byte_t;
typedef byte_t byte;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BYTE				*PBYTE;
typedef BYTE	            *LPBYTE;
typedef int		            *PINT;
typedef int		            *LPINT;
typedef WORD				*PWORD;
typedef WORD	            *LPWORD;
typedef long	            *LPLONG;
typedef DWORD				*PDWORD;
typedef DWORD				*LPDWORD;
typedef void	            *LPVOID;
typedef const void			*LPCVOID;
typedef int                 INT;
typedef unsigned int        *PUINT;
typedef long				LONG_PTR;
typedef unsigned long		ULONG_PTR;

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

typedef u_short n_short;		/* short as received from the net */
typedef u_long	n_long;			/* long as received from the net */
typedef	u_long	n_time;	   /* ms since 00:00 GMT, byte rev */

#define IN
#define OUT
#define WPARAM DWORD
#define LPARAM DWORD
#define AFXAPI
#define PASCAL
#define AFX_CDECL
#define FASTCALL
#define AFX_STATIC_DATA
#define AFX_COMDAT
#define AFX_DATADEF
#define _AFX_INLINE
#define AFX_INLINE inline
#define DWORD_PTR DWORD

#else

#include <Windows.h>

#endif 

#define Singleton(classname) \
public: \
	static classname& getInstance() \
{ \
	static classname _instance; \
	return _instance; \
} \
	protected: \
	classname(); \
	~classname(); \
	classname(const classname& ref); \
	classname& operator = (const classname& ref); \


class DisableCopy
{
private:
	DisableCopy(const DisableCopy&){}
	DisableCopy& operator=(const DisableCopy&){ return *this; }

public:
	DisableCopy(){}
	~DisableCopy(){}
};

#endif