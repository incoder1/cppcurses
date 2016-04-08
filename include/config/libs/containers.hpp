#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <algorithm>

#ifdef CURSES_USE_STDLIB

#	include <functional>
#	include "intrusive_ptr.hpp"
#	include <unordered_set>
#	include <utility>

namespace curses {
	using namespace std::placeholders;
	using ::std::move;
	using ::std::forward;

} // namesapce curses

// extension for enumerations hash
namespace std {
  template<class E>class hash {
    using sfinae = typename std::enable_if<std::is_enum<E>::value, E>::type;
  public:
    size_t operator()(const E&e) const {
      return std::hash<typename std::underlying_type<E>::type>()(e);
    }
  };
} // namespace std

#	define CURSES_HASH std::hash

#	define CURSES_UNORDERED_SET(T) std::unordered_set< T, std::hash<T>, std::equal_to<T>, std::allocator<T> >

#	define CURSES_FUNCTOR std::function
#	define CURSES_BIND std::bind

#	define CURSES_MOVE(BASE_TYPE, ARG) ::std::move( (BASE_TYPE&)(ARG) )

// BOOST option
#else

// smallobject library when has boost
#include <sys_allocator.hpp>

// boost usage
#	include <boost/bind.hpp>
#	include <boost/function.hpp>
#	include <boost/functional/hash.hpp>
#	include <boost/intrusive_ptr.hpp>
#	include <boost/move/move.hpp>
#	include <boost/unordered_set.hpp>

#	define CURSES_HASH boost::hash
#	define CURSES_UNORDERED_SET(T) boost::unordered::unordered_set< T, boost::hash<T>, std::equal_to<T>, smallobject::sys::allocator<T> >
#	define CURSES_FUNCTOR boost::function
#	define CURSES_BIND boost::bind

#	define CURSES_MOVE(BASE_TYPE, ARG) ::boost::move( (BASE_TYPE&)(ARG) )

namespace curses {

	template <typename T> using intrusive_ptr = boost::intrusive_ptr<T>;

	using namespace boost::placeholders;

    using ::boost::move;
	using ::boost::forward;

} // namespace curses

#endif // CURSES_USE_STDLIB

#include <string>
#include <memory>
namespace curses {
	typedef std::basic_string<char_t> string;
} // namesapce curses
