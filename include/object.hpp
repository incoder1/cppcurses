#include "config.hpp"

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CURSES_OBJECT_HPP_INCLUDED__
#define __CURSES_OBJECT_HPP_INCLUDED__

namespace curses {

/// ! \brief a basic class for object which must be stored in heap
/// Extending object adds ability to reference pointer from intrusive_ptr
/// without defining add_ref/release functions
/// Object also provides system depended memory allocation optimized for small heap objects,
/// i.e. windows implementation will uses the private heap, when linux uses glibc malloc/free
class CURSES_PUBLIC object
{
	CURSES_MOVABLE_BUT_NOT_COPYABLE(object)
	protected:
		object() CURSES_NOEXCEPT;
	public:
		virtual ~object() CURSES_NOEXCEPT = 0;
#if defined(CURSES_WIN) || defined(CURSES_JEMALLOC)
		void* operator new(std::size_t s) CURSES_THROW(std::bad_alloc);
		void operator delete(void* const p) CURSES_NOEXCEPT;
#endif // custom memory allocator
	private:
#ifdef CURSES_HAS_ATOMIC
		atomic_size_t refCount_;
#else
		std::size_t refCount_;
#endif // CURSES_HAS_ATOMIC
		friend void intrusive_ptr_add_ref(object* const);
		friend void intrusive_ptr_release(object* const);
};

} // namesapace curses

/// Declaring smart pointer type definition
#define CURSES_DECLARE_SPTR(TYPE) typedef curses::intrusive_ptr<TYPE> sp_##TYPE

#endif // __CURSES_OBJECT_HPP_INCLUDED__
