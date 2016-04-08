#ifndef ___BASIC_TERMINAL_SPECIFICATION_HPP_INCUDED__
#define ___BASIC_TERMINAL_SPECIFICATION_HPP_INCUDED__

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "events.hpp"
#include "geometry.hpp"
#include "sobject.hpp"

namespace curses {

// no alignment to texel
#ifndef CURSES_HAS_CPP11

#pragma pack(push, 1)
template<typename _attrs_t>
struct basic_texel {
	typedef _attrs_t attrs_t;
	char_t char_code;
	attrs_t attrs;
};
#pragma pack(pop)

#else

template<typename _attrs_t>
struct alignas(8) basic_texel {
	typedef _attrs_t attrs_t;
	char_t char_code;
	attrs_t attrs;
};

#endif // CURSES_HAS_CPP11

template <typename _color_t>
struct basic_text_color
{
	typedef _color_t color_t;
	_color_t foreground;
	_color_t background;
	basic_text_color(_color_t f,_color_t b):
		 foreground(f),
		 background(b)
	{}
};

} // curses

#endif // ___BASIC_TERMINAL_SPECIFICATION_HPP_INCUDED__
