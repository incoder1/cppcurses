#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <algorithm>

#ifdef CURSES_USE_STDLIB

#	include <functional>
#	include "intrusive_ptr.hpp"
#	include <unordered_set>
#	include <utility>

#	define CURSES_HASH std::hash

#	define CURSES_UNORDERED_SET(T) std::unordered_set< T, std::hash<T>, std::equal_to<T>, allocator<T> >

#	define CURSES_FUNCTOR std::function
#	define CURSES_BIND std::bind

#	define CURSES_MOVE_BASE(BASE_TYPE, ARG) ::std::move( (BASE_TYPE&)(ARG) )

namespace curses {

	using namespace std::placeholders;

	using ::std::move;
	using ::std::forward;

} // namesapce curses

// BOOST option
#else
// boost usage
#	include <boost/bind.hpp>
#	include <boost/function.hpp>
#	include <boost/functional/hash.hpp>
#	include <boost/intrusive_ptr.hpp>
#	include <boost/move/move.hpp>
#	include <boost/unordered_set.hpp>

#	define CURSES_HASH boost::hash
#	define CURSES_UNORDERED_SET(T) boost::unordered::unordered_set< T, boost::hash<T>, std::equal_to<T>, allocator<T> >
#	define CURSES_FUNCTOR boost::function
#	define CURSES_BIND boost::bind

#	define CURSES_MOVE_BASE(BASE_TYPE, ARG) ::boost::move( (BASE_TYPE&)(ARG) )

namespace curses {

	template <typename T> using intrusive_ptr = boost::intrusive_ptr<T>;

	using namespace boost::placeholders;

    using ::boost::move;
	using ::boost::forward;

} // namespace curses

#endif // CURSES_USE_STDLIB

// memory alloctors options
#ifdef CURSES_JEMALLOC
# 	include "jeallocator.hpp"
#else

#include <memory>
namespace curses {
	template <typename T> using allocator = std::allocator<T>;
} // namesapce curses

#endif // CURSES_JEMALLOC
