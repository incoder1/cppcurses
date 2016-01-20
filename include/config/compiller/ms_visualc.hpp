#if _MSC_VER < 1600
# error "At least Visual C++ version 10 is requared"
#endif // _MSC_VER

// no requared C++ 11 features unless Visual C++ 13
#if _MSC_VER >= 1800
#	define	CURSES_HAS_CPP11
#endif // CPP11 detection

// pragma once supported by default
#	define CURSES_HAS_PRAGMA_ONCE

// excepions mode
#ifndef _CPPUNWIND
#  define CURSES_NO_EXCEPTIONS
#endif

// RTTI mode
#ifdef _CPPRTTI
#	define CURSES_HAS_RTTI
#endif // _CPPRTTI

// Macros to support in mixed 03/0x mode.

#ifdef CURSES_HAS_CPP11
#	define CURSES_THROW(_EXC)
#	define CURSES_NOEXCEPT_IF(Predicate) noexcept((Predicate))
#	define CURSES_OVERRIDE override
#	define CURSES_FINAL final
# else
#	define CURSES_THROW(_EXC) throw(_EXC)
#	define CURSES_NOEXCEPT_IF(Predicate)
#	define CURSES_OVERRIDE
#	define CURSES_FINAL
#endif

#define CURSES_RV_REF(T) T&&

#if _MSC_VER >= 1900
#	define CURSES_NOEXCEPT noexcept
#else
#	define CURSES_NOEXCEPT throw()
#endif // NOEXCEPT

