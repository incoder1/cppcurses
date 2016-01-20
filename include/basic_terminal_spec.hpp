#ifndef ___BASIC_TERMINAL_SPECIFICATION_HPP_INCUDED__
#define ___BASIC_TERMINAL_SPECIFICATION_HPP_INCUDED__

#include "events.hpp"
#include "geometry.hpp"
#include "object.hpp"

namespace curses {

template<typename _attrs_t>
struct basic_texel {
	typedef _attrs_t attrs_t;
	char_t char_code;
	attrs_t attrs;
};


template <typename _color_t>
struct basic_text_color
{
	typedef _color_t color_t;
	_color_t foreground;
	_color_t background;
	basic_text_color(_color_t f,_color_t b):
		 foreground(f),
		 background(b)
	{}
};

template<typename _texel, typename _text_color,class _implementor>
class basic_terminal: public virtual object {
private:
	typedef _implementor implementor;
protected:
	typedef typename _texel::attrs_t attrs_t;
	basic_terminal() CURSES_NOEXCEPT
	{}
public:
	typedef _texel texel_type;

	void putch(const position& pos, char_t ch) const {
		implementor::putch(pos,ch);
	}

	void put_line(const position& pos, char_t ch, uint8_t count) const {
		implementor::put_line(pos,ch,count);
	}

	uint8_t put_text(const position& pos, const char_t* str) const {
		return implementor::put_text(pos,str);
	}

	bounds get_bounds() const {
		return implementor::get_bouns();
	}

	void set_size(uint8_t width, uint8_t height) const {
		implementor::set_size(width,height);
	}

	void fill_rectangle(const rectangle& rect,char_t ch) const {
		implementor::fill_rectangle(rect,ch);
	}

	void set_color(const _text_color& color) const
	{
		return implementor::set_color(color);
	}

	_text_color current_color() const {
		return implementor::current_color();
	}

	void clipt_rect(const rectangle& rect, texel_type* buff) const {
		return implementor::copy_region(rect);
	}

	void paste_rect(const rectangle& rect, texel_type* data) const {
		implementor::paste_rectangle(rect,data);
	}

	event wait_for_event() const
	{
		return implementor::wait_for_event();
	}
};

} // curses

#endif // ___BASIC_TERMINAL_SPECIFICATION_HPP_INCUDED__
