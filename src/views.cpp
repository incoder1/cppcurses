#include "stdafx.hpp"
#include "views.hpp"

namespace curses {

// view
view::view(sp_pen&& p, rectangle&& rect) CURSES_NOEXCEPT:
	parent_( CURSES_MOVE_BASE(sp_pen, p) ),
	rect_( CURSES_MOVE_BASE(rectangle, rect) ),
	display_(),
	underground_(NULL)
{
	display_ = parent_->sub_pen(rect_);
}

view::~view() CURSES_NOEXCEPT {
	hide();
}

void view::copy_underground()
{
	underground_ = parent_->clipt_rect(display_->get_relative());
}

void view::restore_underground()
{
	parent_->paste_rect(display_->get_relative(), underground_);
	underground_ = NULL;
}

void view::set_position_to(const position& pos)
{
	hide();
	rect_.move_to(pos.x,pos.y);
	display_ = parent_->sub_pen(rect_);
	show();
}

void view::resize(const bounds& bds)
{
	hide();
	rect_.resize(bds.width, bds.height);
	display_ = parent_->sub_pen(rect_);
	show();
}

sp_pen view::get_pen() const
{
	return display_;
}

void view::hide()
{
	if( visible() ) {
		restore_underground();
	}
}

void view::show()
{
	if( !visible() ) {
		copy_underground();
		this->fill();
	}
}

// box_view
box_view::box_view(sp_pen p, const rectangle& rect, const text_color& color):
	view( move(p), CURSES_MOVE_BASE(rectangle,rect) ),
	body_color_(color)
{
}

box_view::box_view(sp_pen&& p, rectangle&& rect, text_color&& color):
	view(forward<sp_pen>(p),forward<rectangle>(rect)),
	body_color_( CURSES_MOVE_BASE(text_color,color) )
{
}

box_view::~box_view() CURSES_NOEXCEPT
{
	hide();
}

void box_view::fill() {
	sp_pen p = display_pen();
	p->set_color(body_color_);
	bounds bds = rect().get_bounds();
	p->out_box(0, 0, bds.width, bds.height, TEXT(' ') );
}


// bordered_box_view
bordered_box_view::bordered_box_view(sp_pen p,const rectangle& rect,const text_color& color,sp_border brd):
	box_view(move(p),CURSES_MOVE_BASE(rectangle,rect),CURSES_MOVE_BASE(text_color,color)),
	border_(brd)
{}

bordered_box_view::~bordered_box_view() CURSES_NOEXCEPT
{}

sp_pen bordered_box_view::get_pen() const
{
	bounds bds = rect().get_bounds();
	return display_pen()->sub_pen( 1, 1, bds.width-1, bds.height-1);
}

void bordered_box_view::fill() {
	box_view::fill();
	border_->out(display_pen().get(), rect().get_bounds());
}

} // namespace curses
