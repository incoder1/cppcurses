#ifndef __CURSES_TEXTJET_HPP_INCLUDED__
#define __CURSES_TEXTJET_HPP_INCLUDED__

#include "object.hpp"
#include "views.hpp"

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace curses {

// textjet
class CURSES_PUBLIC textjet:public virtual object
{
protected:
	explicit textjet(sp_view view) CURSES_NOEXCEPT;
	inline sp_view view() const {
		return view_;
	}
public:
	virtual ~textjet() CURSES_NOEXCEPT = 0;
	inline void show() {
		view_->show();
	}
	inline void hide() {
		view_->hide();
	}
	inline void set_position_to(uint8_t x, uint8_t y) {
		view_->set_position_to( position(x,y) );
	}
	inline void resize(uint8_t w, uint8_t h) {
		return view_->resize( bounds(w,h) );
	}
private:
	sp_view view_;
};

CURSES_DECLARE_SPTR(textjet);

} // namespace curses

#endif // __CURSES_TEXTJET_HPP_INCLUDED__
