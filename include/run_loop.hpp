#ifndef __CURSES_RUNLOOP_HPP_INCLUDED__
#define __CURSES_RUNLOOP_HPP_INCLUDED__

#include "action.hpp"
#include "events.hpp"
#include "terminal.hpp"

namespace curses {

class run_loop CURSES_FINAL:private curses::noncopyable
{
	public:
		run_loop(sp_terminal trm) CURSES_NOEXCEPT;
		void start();
#ifdef CURSES_HAS_CPP11
		~run_loop() = default;
#endif // CURSES_HAS_CPP11
	private:
        sp_terminal term_;
};

} // namespace curses

#endif // __CURSES_RUNLOOP_HPP_INCLUDED__
