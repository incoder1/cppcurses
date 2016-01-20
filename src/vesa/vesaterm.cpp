#include "../stdafx.hpp"
#include "vesaterm.hpp"

namespace curses {

namespace vesa {

void initialize(vesa_mode mode)
{
}

void release()
{
}

} // namespeace vesa

// Default address of vide text buffer
texel** terminal::_vbuff = reinterpret_cast<texel**>(0xB800);

static CURSES_FORCEINLINE uint8_t color_to_attr(const text_color& color)
{
	return  (color.background << 4) | color.foreground;
}

CURSES_FORCEINLINE text_color attrs_to_color(uint8_t attrs)
{
	uint8_t foreground = attrs & 0x0F;
	uint8_t background = attrs >> 4;
	return text_color( foreground, background);
}

// terminal
explicit terminal::terminal(vesa_mode mode)
{
	vesa::initialize(mode);
}

virtual terminal::~terminal() CURSES_NOEXCEPT {
	vesa::release();
}

bounds terminal::get_bounds() const
{
}

void terminal::set_size(uint8_t width, uint8_t height) const
{
}

void terminal::putch(const position& pos,char_t ch) const
{
	_vbuff[pos.x,pos.y]->char_code = ch;
	_vbuff[pos.x,pos.y]->attrs = currentAttrs_;
}

void terminal::put_line(const position& pos,char_t ch, uint8_t count) const
{
	for(uint8_t i = 0; i < count; i++) {
		putch(pos.x++,pos.y,ch);
	}
}

uint8_t terminal::put_text(const position& pos, const char_t* str) const
{
	char_t *it = const_cast<char_t*>(str);
	position outp = pos;
	do {
		putch(outp,*it);
		++outp.x;
		++it;
	} while(*it != 0);
	return static_cast<uint8_t>(const_cast<const char_t*>(it)-str);
}

void terminal::fill_rectangle(const rectangle& r,char_t ch) const
{
	position pos = r.left_top();
	bounds bds = r.get_bounds();
	uint8_t count = bds.width;
	uint8_t lines = r.left() + bds.height;
	for(uint8_t y = r.top(); y < lines; y++ ) {
		pos.y = y;
		put_line(pos,ch,count);
	}
}

void terminal::set_color(const text_color& color) const
{
	currentAttrs_ = color_to_attr(color);
}

text_color terminal::current_color() const
{
	return attrs_to_color(currentAttrs_);
}

void terminal::clipt_rect(const rectangle& rect,texel_type* buff) const
{
}

void terminal::paste_rect(const rectangle& rect, texel_type* data) const
{
}

event terminal::wait_for_event() const
{
}

} // namesapce curses
