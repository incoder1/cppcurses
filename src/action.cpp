#include "stdafx.hpp"
#include "action.hpp"

namespace curses {

// action
action::action(event_handler&& handler) CURSES_NOEXCEPT:
	handler_( CURSES_MOVE_BASE(event_handler,handler) )
{}

action::~action() CURSES_NOEXCEPT
{}

// signal
signal::signal(sp_action action) CURSES_NOEXCEPT:
	action_(action)
{}

#ifndef CURSES_HAS_CPP11
signal::~signal() CURSES_NOEXCEPT
{}
#endif

void signal::emit(const event& e) const
{
	action_->emit(e);
}

} // namespace curses
