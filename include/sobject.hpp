#ifndef __CURSES_OBJECT_HPP_INCLUDED__
#define __CURSES_OBJECT_HPP_INCLUDED__

#include "config.hpp"

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef CURSES_USE_STDLIB
// use small object allocator if available
#include <object.hpp>
namespace curses {
	using namespace smallobject;
} // namespace curses
#else


namespace curses {

class object {
#if defined(CURSES_HAS_CPP11)
	object( const object& ) = delete;
	object& operator=( const object& ) = delete;
#else
  private:  // emphasize the following members are private
      object( const object& );
      object& operator=( const object& );
#endif // no deleted functions
protected:
    object() CURSES_NOEXCEPT
    {}
public:
	virtual ~object() CURSES_NOEXCEPT
	{}
private:
	atomic_size_t ref_count_;
	friend CURSES_FORCEINLINE void intrusive_ptr_add_ref(object* const obj);
	friend CURSES_FORCEINLINE void intrusive_ptr_release(object* const obj);
};

CURSES_FORCEINLINE void intrusive_ptr_add_ref(object* const obj)
{
	obj->ref_count_.fetch_add(1, memory_order::memory_order_relaxed);
}

CURSES_FORCEINLINE void intrusive_ptr_release(object* const obj)
{
	 if (obj->ref_count_.fetch_sub(1, memory_order::memory_order_release) == 1) {
      atomic_thread_fence(memory_order::memory_order_acquire);
      delete obj;
    }
}

} // namespace curses
#endif // defined

/// Declaring smart pointer type definition
#define CURSES_DECLARE_SPTR(TYPE) typedef curses::intrusive_ptr<TYPE> sp_##TYPE

#endif // __CURSES_OBJECT_HPP_INCLUDED__
