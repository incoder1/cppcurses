#ifndef __CURSES_MENU_HPP_INCLUDED__
#define __CURSES_MENU_HPP_INCLUDED__

#include "events.hpp"
#include "sobject.hpp"
#include "action.hpp"

#include <string>

namespace curses {

enum item_type {
	MENU,
	ITEM,
	SEPARATOR
};


class menu_item {
public:
#ifndef CURSES_HAS_CPP11
	menu_item(const string& caption):
		caption_(caption)
	{
	}
#else
	menu_item(string&& caption):
		caption_(CURSES_MOVE(string,caption))
	{}
#endif
private:
	string caption_;
};


} // namesapce curses

#endif // __CURSES_MENU_HPP_INCLUDED__
