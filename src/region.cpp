#include "stdafx.hpp"
#include "region.hpp"

namespace curses {

// region
region::region(const rectangle& limit):
	limit_(limit),
	visible_(false)
{
	bounds bds = limit_.get_bounds();
	visible_ = bds.width > 0 || bds.height > 0;
}

region::region(int8_t l, int8_t t, int8_t w, int8_t h):
	limit_(l,t,w,h),
	visible_(false)
{
	bounds bds = limit_.get_bounds();
	visible_ = bds.width > 0 || bds.height > 0;
}


uint8_t region::visible_line_len(const position& pos, uint8_t lenght) const
{
	uint8_t result = 0;
	if(is_pos_visible(pos)) {
		uint8_t maxlen = limit_.right()+1 - pos.x;
		if(lenght <= maxlen) {
			result = lenght;
		} else {
			result = maxlen;
		}
	}
	return result;
}

uint8_t region::visible_text_len(const position& pos, const char_t* str) const
{
	uint8_t result = 0;
	if(is_pos_visible(pos)) {
		uint8_t limit = limit_.get_bounds().width;
		char_t *ch = const_cast<char_t*>(str);
		while( (*ch != 0) && (result < limit) ) {
			++result;
			++ch;
		}
	}
	return result;
}


rectangle region::get_visible_box(uint8_t x,uint8_t y, uint8_t w, uint8_t h) const {
	position lt = current_to_parent(x,y);
	if(is_pos_visible(lt)) {
		position rb;
		rb.x = lt.x + (w-1);
		if(rb.x > limit_.right()) {
			rb.x = limit_.right();
		}
		rb.y = lt.y + (h-1);
		if(rb.y > limit_.bottom()) {
			rb.y = limit_.bottom();
		}
		return rectangle(lt,rb);
	}
	return rectangle(0,0,0,0);
}

} // namespace curses

