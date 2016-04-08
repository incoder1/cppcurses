#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CURSES_JEMALLOC_ALLOCTOR_HPP_INCLUDED__
#define __CURSES_JEMALLOC_ALLOCTOR_HPP_INCLUDED__

#include <jemalloc/jemalloc.h>
#include <string>

namespace curses {

template<typename T>
class allocator {
public:
	//    typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public :
	//    convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef allocator<U> other;
	};

public :

	CURSES_CONSTEXPR allocator()
	{}

	template<typename U>
	inline allocator(allocator<U> const&)
	{}

	//    address
	inline pointer address(reference r)
	{
		return &r;
	}
	inline const_pointer address(const_reference r)
	{
		return &r;
	}

	//    memory allocation
	inline pointer allocate(size_type cnt)
	{
		return reinterpret_cast<pointer>(::je_malloc(cnt * sizeof (T)));
	}

	inline void deallocate(pointer p, size_type)
	{
		::je_free(p);
	}

	//    size
	inline size_type max_size() const
	{
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	//    construction/destruction
	inline void construct(pointer p, const T& t)
	{
		new(p) T(t);
	}
	inline void destroy(pointer p)
	{
		p->~T();
	}

	inline bool operator==(allocator const&)
	{
		return true;
	}
	inline bool operator!=(allocator const& a)
	{
		return !operator==(a);
	}
};

typedef std::basic_string<char_t, std::char_traits<char_t>, curses::allocator<char_t> > string;

} // namespace curses

#endif // __CURSES_JEMALLOC_ALLOCTOR_HPP_INCLUDED__
