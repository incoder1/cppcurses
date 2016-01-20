#include "config.hpp"

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CURSES_VIEWS_HPP_INCUDED__
#define __CURSES_VIEWS_HPP_INCUDED__

#include "border.hpp"
#include "object.hpp"
#include "pen.hpp"
#include "terminal.hpp"

namespace curses {

class CURSES_PUBLIC view:public virtual object
{
protected:
	view(sp_pen&& p, rectangle&& rect) CURSES_NOEXCEPT;
public:
	virtual ~view() CURSES_NOEXCEPT = 0;
	void hide();
	void show();
	void set_position_to(const position& pos);
	void resize(const bounds& bds);
	virtual sp_pen get_pen() const;
	inline bool visible() const {
		return underground_ != NULL;
	}
	inline rectangle rect() const {
		return rect_;
	}
private:
	void restore_underground();
	void copy_underground();
protected:
	virtual void fill() = 0;
	sp_pen display_pen() const {
		return display_;
	}
private:
	sp_pen parent_;
	rectangle rect_;
	sp_pen display_;
	texel* underground_;
};

CURSES_DECLARE_SPTR(view);

// boxed views

class CURSES_PUBLIC box_view:public view {
public:
	box_view(sp_pen p, const rectangle& rect, const text_color& color);
	virtual ~box_view() CURSES_NOEXCEPT;
protected:
	box_view(sp_pen&& p, rectangle&& rect, text_color&& color);
	virtual void fill() CURSES_OVERRIDE;
private:
	text_color body_color_;
};

class bordered_box_view:public box_view
{
public:
	bordered_box_view(sp_pen p,const rectangle& rect,const text_color& color,sp_border brd);
	virtual ~bordered_box_view() CURSES_NOEXCEPT;
	virtual sp_pen get_pen() const CURSES_OVERRIDE;
protected:
	virtual void fill() CURSES_OVERRIDE;
private:
	sp_border border_;
};


// line views
//class line_view:public box_view {
//public:
//	line_view(const terminal* term,
//			  const position& p,
//			  uint8_t length,
//			  const text_color& body_color);
//};

} // namespace curses

#endif // __CURSES_VIEWS_HPP_INCUDED__
