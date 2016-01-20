#if defined(CURSES_HAS_CPP11) && !defined(CURSES_FORCE_USE_BOOST)
#	define CURSES_USE_STDLIB
#endif // CURSES_USE_STDLIB

// Unix detection
#if defined(unix) \
      || defined(__unix) \
      || defined(_XOPEN_SOURCE) \
      || defined(_POSIX_SOURCE)
#	include "unix.hpp"
#endif // defined

// Windows detection
#if ( defined(_WIN32) || defined(_WIN64) )
#	include "windows.hpp"
#endif // Windows detection

// If platform not providing any specific declspec, define as empty constant
#ifndef CURSES_PUBLIC
#	define CURSES_PUBLIC
#endif //

// option to use je malloc memory allocator for curses objects
// for FreeBSD it is already jemalloc
#if defined(CURSES_JEMALLOC) && defined(__FreeBSD__)
#	undef CURSES_JEMALLOC
#endif // CURSES_JEMALLOC
