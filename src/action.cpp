#include "stdafx.hpp"
#include "action.hpp"

namespace curses {

// action
action::action(event_handler&& handler) CURSES_NOEXCEPT:
	handler_( CURSES_MOVE(event_handler,handler) )
{}

action::~action() CURSES_NOEXCEPT
{}

// signal
signal::signal(const sp_action& action) CURSES_NOEXCEPT:
	action_(action)
{}

void signal::emit(const event& e) const
{
	action_->emit(e);
}

} // namespace curses
