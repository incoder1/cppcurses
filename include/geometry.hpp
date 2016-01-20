#ifndef __GEOMETRY_HPP_INCLUDED__
#define __GEOMETRY_HPP_INCLUDED__

#include <cstdint>

namespace curses {

struct position {
	uint8_t x,y;
	position():
		x(0),y(0)
	{}
	position(uint8_t X, uint8_t Y):
		x(X),y(Y)
	{}
};

struct bounds {
	uint8_t width;
	uint8_t height;
	bounds():
		width(0),
		height(0)
	{}
	bounds(uint8_t w, uint8_t h):
		width(w),
		height(h)
	{}
	uint32_t size() const {
		return width*height;
	}
};

class rectangle {
public:
	rectangle() CURSES_NOEXCEPT:
		left_(0),top_(0),right_(0),bottom_(0)
	{}
	rectangle(const position& lt, const position& rb) CURSES_NOEXCEPT:
		left_(lt.x),
		top_(lt.y),
		right_(rb.x),
		bottom_(rb.y)
	{}
	rectangle(const position& p, const bounds& b) CURSES_NOEXCEPT:
		left_(p.x),
		top_(p.y),
		right_(p.x+(b.width-1)),
		bottom_(p.y+(b.height-1))
	{}
	rectangle(uint8_t left, uint8_t top, uint8_t width, uint8_t height) CURSES_NOEXCEPT:
		left_(left),
		top_(top),
		right_(left+(width-1)),
		bottom_(top+(height-1))
	{}
	position left_top() const {
		return position(left_,top_);
	}
	position right_bottom() {
		return position(right_,bottom_);
	}
	bounds get_bounds() const {
		return bounds( (right_ - left_)+1, (bottom_ - top_)+1 );
	}
	inline uint8_t left() const {
		return left_;
	}
	inline uint8_t top() const {
		return top_;
	}
	inline uint8_t right() const {
		return right_;
	}
	inline uint8_t bottom() const {
		return bottom_;
	}
	void move_to(uint8_t x, uint8_t y) {
		left_ = x;
		top_ = y;
		right_ += x;
		bottom_ += y;
	}
	void resize(uint8_t width, uint8_t height) {
		right_ = left_ + (width-1);
		bottom_ = top_ + (height-1);
	}
private:
	uint8_t left_;
	uint8_t top_;
	uint8_t right_;
	uint8_t bottom_;
};

} // curses

#endif // __GEOMETRY_HPP_INCLUDED__
