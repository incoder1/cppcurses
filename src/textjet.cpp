#include "stdafx.hpp"
#include "textjet.hpp"

namespace curses {

textjet::textjet(sp_view view) CURSES_NOEXCEPT:
	view_(view)
{}

textjet::~textjet() CURSES_NOEXCEPT
{}

} // namespace curses
