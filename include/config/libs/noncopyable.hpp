#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CURSES_NONCOPYABLE_HPP_INCLUDED__
#define __CURSES_NONCOPYABLE_HPP_INCLUDED__

#include "config.hpp"

/// Macro for hide/delete the private constructor and assign operator
#ifdef CURSES_HAS_CPP11
#	define CURSES_MOVABLE_BUT_NOT_COPYABLE(__CLASS) \
			__CLASS(const __CLASS&) = delete;\
			__CLASS& operator=(const __CLASS&) = delete;
#else
#	define CURSES_MOVABLE_BUT_NOT_COPYABLE(__CLASS) \
		private:\
		 __CLASS(const __CLASS&) {}\
		 __CLASS& operator=(const __CLASS&) {return *this;}
#endif // CURSES_HAS_CPP11

namespace curses {

/// ! \brief private extending of noncopyable
/// allow you not to hide/delete copy constructor and
/// assign operators in the target class
class noncopyable
{
	CURSES_MOVABLE_BUT_NOT_COPYABLE(noncopyable)
protected:
#ifdef CURSES_HAS_CPP11
	noncopyable() = default;
	~noncopyable() = default;
#else
      noncopyable() {}
      ~noncopyable() {}
#endif // CURSES_HAS_CPP11
};


} // namespace curses

#endif // __CURSES_NONCOPYABLE_HPP_INCLUDED__
