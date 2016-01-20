#ifndef __CURSES_MENU_HPP_INCLUDED__
#define __CURSES_MENU_HPP_INCLUDED__

#include "events.hpp"
#include "object.hpp"
#include "terminal.hpp"

namespace curses {

class menu_item:public virtual object
{
public:
	menu_item(const char_t* name):
		name_(name)
	{}
private:
	const char_t* name_;
};

CURSES_DECLARE_SPTR(menu_item);

class menu:public virtual object
{
	public:
		explicit menu();
		virtual ~menu();
};

CURSES_DECLARE_SPTR(menu);

} // namesapce curses

#endif // __CURSES_MENU_HPP_INCLUDED__
