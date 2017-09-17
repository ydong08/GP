

#pragma once

#define DD_PLATFORM_WIN32	1
#define DD_PLATFORM_OSX		2
#define DD_PLATFORM_LINUX	3

#if defined(_MSC_VER)
#	define DD_PLATFORM DD_PLATFORM_WIN32
#	define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#elif defined(__CLANG__)
#	define DD_PLATFORM DD_PLATFORM_OSX
#elif defined(__GNUC__)
#	define DD_PLATFORM DD_PLATFORM_LINUX
#endif

//dll define
#if DD_PLATFORM == DD_PLATFORM_WIN32
#	if defined(DD_MODULE)
#		define DDExport	__declspec( dllexport )
#	else
#		define DDExport	__declspec( dllimport )
#	endif
#else
#		define DDExport
#endif