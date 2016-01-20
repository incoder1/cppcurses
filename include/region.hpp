#ifndef __REGION_HPP_INCLUDED__
#define __REGION_HPP_INCLUDED__

#include "geometry.hpp"

namespace curses {

class CURSES_PUBLIC region
{
public:
	explicit region(const rectangle& limit);
	region(int8_t l, int8_t t, int8_t w, int8_t h);

	uint8_t visible_line_len(const position& pos, uint8_t lenght) const;
	rectangle get_visible_box(uint8_t x,uint8_t y, uint8_t w, uint8_t h) const;
	uint8_t visible_text_len(const position& pos, const char_t* str) const;

	inline rectangle get_visible_box(const rectangle& rect) const {
		bounds bds = rect.get_bounds();
		return get_visible_box(rect.left(),rect.top(),bds.width,bds.height);
	}

	inline position current_to_parent(const position& pos) const
	{
		return position( limit_.left() + pos.x, limit_.top() + pos.y );
	}

	inline position current_to_parent(uint8_t x, uint8_t y) const
	{
        return current_to_parent(position(x,y));
	}

	inline position parent_to_current(const position& pos) const
	{
		return position( pos.x - limit_.left(), pos.y - limit_.top() );
	}

	inline position parent_to_current(uint8_t x, uint8_t y) const
	{
		return parent_to_current( position(x,y) );
	}

	inline bool is_pos_visible(const position& pos) const
	{
		return ( pos.x <= limit_.right() ) && ( pos.y <= limit_.bottom() );
	}

	inline rectangle get_limit() const {
		return limit_;
	}

	inline bool is_visible() const {
		return visible_;
	}

private:
	rectangle limit_;
	bool visible_;
};


} // namesapce curses

#endif // __REGION_HPP_INCLUDED__
