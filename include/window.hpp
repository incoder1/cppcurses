#ifndef __CURSES_WINDOW_HPP_INCLUDED__
#define __CURSES_WINDOW_HPP_INCLUDED__

#include "views.hpp"

namespace curses {

class application;

class window {
public:
	window()
	{
	}
	virtual void show() {
	}
private:
};

} // namesapce curses

#endif // __CURSES_WINDOW_HPP_INCLUDED__
