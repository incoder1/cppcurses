#ifndef __CURSES_ATOMICS_HPP_INCLUDED__
#define __CURSES_ATOMICS_HPP_INCLUDED__

#define CURSES_HAS_ATOMIC 1

// remapping of system atomic
namespace curses {

#if defined(CURSES_HAS_CPP11) && !defined(CURSES_FORCE_USE_BOOST)
#	include <atomic>
	typedef std::atomic_size_t atomic_size_t;
	using std::atomic;
	using std::atomic_thread_fence;
#else
#	include <boost/atomic.hpp>
	typedef boost::atomics::atomic_size_t atomic_size_t;
	using boost:atomics::atomic;
	using boost:atomics::atomic_thread_fence;
#endif // defined

}

#endif // __CURSES_ATOMICS_HPP_INCLUDED__
