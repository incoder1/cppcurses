#ifndef __CURSES_APPLICATION_HPP_INCLUDED__
#define __CURSES_APPLICATION_HPP_INCLUDED__

#include "terminal.hpp"
#include "window.hpp"

namespace curses {

class application {
	CURSES_MOVABLE_BUT_NOT_COPYABLE(application)
public:
	application(sp_pen p):
		p_( p )
	{}
	void run() {
		bounds bds = p_->get_bounds();
		p_->set_color(text_color(BLACK, WHITE) );
		p_->out_line(0, 0,  bds.width,  TEXT(' '));
		p_->out_line(0, bds.height-1, bds.width,  TEXT(' '));
		p_->set_color(text_color(WHITE, GREY));
		p_->out_box(0, 1, bds.width, bds.height, CRS_CLEAR_BODY_CH );
	}

private:
	sp_pen p_;
};

} // curses

#endif // __CURSES_APPLICATION_HPP_INCLUDED__
