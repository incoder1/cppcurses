#define CURSES_WIN

#if !defined(UNICODE) && !defined(_UNICODE)
#	error "Windows implementation requares Unicode Win API. Please specify predefined compiller marcoses"
#endif // unicode check

#if defined(UNICODE) && !defined(_UNICODE)
#	define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#	define UNICODE
#endif // define all posibile

#if defined(__GNUC__) && !defined(WIN32_LEAN_AND_MEAN)
#	define WIN32_LEAN_AND_MEAN
#	define LEAN_AND_MEAN_NEED_RESTORE
#endif // WIN32_LEAN_AND_MEAN

#include "atomics.hpp"

#include <wchar.h>
#include <windows.h>

#ifdef LEAN_AND_MEAN_NEED_RESTORE
#	undef WIN32_LEAN_AND_MEAN
#	undef LEAN_AND_MEAN_NEED_RESTORE
#endif // LEAN_AND_MEAN_NEED_RESTORE

//# ifdef CURSES_BUILD_DLL
//#	define CURSES_PUBLIC __declspec(dllexport)
//#else
//#	define CURSES_PUBLIC __declspec(dllimport)
//#endif

namespace curses {
	// utf-16LE
	typedef wchar_t char_t;
}
