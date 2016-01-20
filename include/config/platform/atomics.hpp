#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#define CURSES_HAS_ATOMIC 1

#if defined(CURSES_USE_STDLIB) && !defined(_MSC_VER)
#	include <atomic>
#elif defined(CURSES_USE_STDLIB) && _MSC_VER > 1800
#	include <atomic>
#else
#	include <boost/atomic/atomic.hpp>
#endif // CURSES_HAS_CPP11

// remapping of system atomic
namespace curses {

#if defined(CURSES_HAS_CPP11) && !defined(CURSES_FORCE_USE_BOOST)
	template<typename T> using atomic = std::atomic<T>;
	using atomic_size_t = std::atomic_size_t;
	using std::atomic_thread_fence;
	using memory_order = std::memory_order;
#else
	template<typename T> using atomic = boost::atomics::atomic<T>;
	using atomic_size_t = boost::atomic_size_t;
	using boost::atomic_thread_fence;
	using memory_order = boost::memory_order;
#endif // defined

} // curses
