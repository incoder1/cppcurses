#define CURSES_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if __cplusplus >= 201103L
#	define	CURSES_HAS_CPP11
#endif // CPP11 detection

#if !defined(CURSES_HAS_CPP11) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
#	error "This library requres at lest C++ 0x standard support, please add -std= GCC option or upgade GCC. CPP 11 and hier is perffered"
#endif // check CPP0X compiller

#ifdef __GXX_RTTI
#	define CURSES_HAS_RTTI
#endif // GCC Rtti

#ifndef  _GLIBCXX_NOEXCEPT
#	define CURSES_NO_EXCEPTIONS
#endif // exception

#if CURSES_GCC_VERSION >= 30400
#	define CURSES_HAS_PRAGMA_ONCE
#endif

// requared with the cpp 0x
#define CURSES_RV_REF(T) T&&

// Macros to support in mixed 11/0x mode.
#ifdef CURSES_HAS_CPP11
#	define CURSES_NOEXCEPT noexcept
#	define CURSES_THROW(_EXC)
#	define CURSES_NOEXCEPT_IF(Predicate) noexcept((Predicate))
#	define CURSES_OVERRIDE override
#	define CURSES_FINAL final
# else
#	define CURSES_NOEXCEPT throw()
#	define CURSES_THROW(_EXC) throw(_EXC)
#	define CURSES_NOEXCEPT_IF(Predicate)
#	define CURSES_OVERRIDE
#	define CURSES_FINAL

#endif

#define CURSES_FORCEINLINE inline __attribute__ ((always_inline))

#define CURSES_PREVENT_MACRO_SUBSTITUTION
