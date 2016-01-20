#ifndef __CURSES_FRAME_HPP_INCUDED__
#define __CURSES_FRAME_HPP_INCUDED__

#include "terminal.hpp"
#include "object.hpp"
#include "border.hpp"

namespace curses {

class frame:public virtual object {
CURSES_MOVABLE_BUT_NOT_COPYABLE(frame)
public:
	frame(const terminal* term,const rect& rect, const text_color& body_color):
		terminal_(term),
		rect_(rect),
		body_color_(body_color),
		underground_(NULL)
	{}
	virtual ~frame() {
		if(NULL != underground_) {
			delete underground_;
		}
	}
protected:
	void set_term_color(const text_color& color) {
		terminal_->set_out_attrs(terminal_->make_attrs(color));
	}
	const terminal* term() {
		return terminal_;
	}
public:
	void move(uint8_t x, uint8_t y) {
		hide();
		uint8_t w = rect_.width()-1;
		uint8_t h = rect_.height()-1;
		rect_.left = x;
		rect_.top = y;
		rect_.right = rect_.left + w;
		rect_.bottom = rect_.top + h;
		show();
	}
	void resize(uint8_t width,uint8_t height) {
		hide();
		rect_.right = rect_.left + (width-1);
		rect_.bottom = rect_.top + (height-1);
		show();
	}
	void hide() CURSES_NOEXCEPT {
		if(NULL != underground_) {
			terminal_->paste_rect(rect_,underground_);
			terminal_->free_buffer(underground_);
		}
	}
	virtual void show() {
		underground_ = terminal_->alloc_buffer(rect_);
		terminal_->clipt_rect(rect_,underground_);
		set_term_color(body_color_);
		terminal_->fill_rect(rect_,TEXT(' '));
	}
	rect get_rect() const {
		return rect_;
	}
private:
	const terminal* terminal_;
	rect rect_;
	text_color body_color_;
	texel* underground_;
};

template<class _border_t>
class bordered_frame:public frame {
public:
	typedef _border_t border_t;
	bordered_frame(const terminal* term,const rect& rect,const text_color& color):
		frame(term,rect,color),
		border_(term,color)
	{}
	virtual void show() {
		frame::show();
		border_.out(get_rect());
	}
private:
	border_t border_;
};

template<class _border_t>
class titled_frame:public bordered_frame<_border_t> {
private:
	typedef bordered_frame<_border_t> base_type;
public:
	titled_frame(const terminal* term,const rect& rect,
				const text_color& color,
				const terminal::char_t* title):
		base_type(term,rect,color),
		title_(title)
	{
	}
	virtual void show() {
		base_type::show();
		rect rect = base_type::get_rect();
		position pos;
		pos.x = rect.left + 1;
		pos.y = rect.top;
		const terminal* trm = base_type::term();
		uint8_t len = trm->put_text(pos,TEXT("[ "));
		pos.x += len;
		len = trm->put_text(pos,title_);
		pos.x += len;
		trm->put_text(pos,TEXT(" ]"));
	}
private:
	const terminal::char_t* title_;
};

} // namespace curses

#endif // __CURSES_FRAME_HPP_INCUDED__
