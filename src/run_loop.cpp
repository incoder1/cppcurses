#include "run_loop.hpp"

namespace curses {

run_loop::run_loop(sp_terminal trm) CURSES_NOEXCEPT:
	term_(trm)
{}

} // namespace curses
