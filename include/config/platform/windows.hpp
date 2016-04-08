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

#ifdef BUILD_SHARED_LIBRARY
#	define CURSES_PUBLIC __declspec(dllexport)
#elif defined(CURSES_LINK_SHARED)
#	define CURSES_PUBLIC __declspec(dllimport)
#endif // BUILD_SHARED_LIBRARY

#include "atomics.hpp"

#include <wchar.h>
#include <windows.h>

#ifdef LEAN_AND_MEAN_NEED_RESTORE
#	undef WIN32_LEAN_AND_MEAN
#	undef LEAN_AND_MEAN_NEED_RESTORE
#endif // LEAN_AND_MEAN_NEED_RESTORE

#define CURSES_TERMINAL_EMULATOR 1

namespace curses {
	// utf-16LE
	typedef wchar_t char_t;

}

// coreners defs

#define CRS_SINLE_TOP_LEFT     L'┌'
#define CRS_SINLE_TOP_RIGHT    L'┌'
#define CRS_SINLE_BOTTOM_LEFT  L'└'
#define CRS_SINLE_BOTTOM_RIGHT L'┘'
#define CRS_SINLE_HORIZONTAL   L'─'
#define CRS_SINLE_VERTICAL     L'│'

#define CRS_DOUBLE_TOP_LEFT     L'╔'
#define CRS_DOUBLE_TOP_RIGHT    L'╗'
#define CRS_DOUBLE_BOTTOM_LEFT  L'╚'
#define CRS_DOUBLE_BOTTOM_RIGHT L'╝'
#define CRS_DOUBLE_HORIZONTAL   L'═'
#define CRS_DOUBLE_VERTICAL     L'║'

#define CRS_SINGLE_TABLE_LEFT   L'┤'
#define CRS_SINGLE_TABLE_RIGHT  L'├'
#define CRS_SINGLE_TABLE_MIDLE  L'┼'
#define CRS_SINGLE_TABLE_TOP    L'┬'
#define CRS_SINGLE_TABLE_BOTTOM L'┴'

#define CRS_DOUBLE_TABLE_LEFT   L'╣'
#define CRS_DOUBLE_TABLE_RIGHT  L'╠'
#define CRS_DOUBLE_TABLE_MIDLE  L'╬'
#define CRS_DOUBLE_TABLE_TOP    L'╦'
#define CRS_DOUBLE_TABLE_BOTTOM L'╩'

#define CRS_CLEAR_BODY_CH       L'░'
