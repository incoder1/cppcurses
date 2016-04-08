#include "stdafx.hpp"
#include "border.hpp"

namespace curses {

namespace detail {


//#elif defined(CURSES_UNIX)
//

//#else
//// VESA EGA, VGA etc

//#endif // UNICODE

struct single_line_corners {
private:
	typedef char_t char_t;
	single_line_corners() {}
public:
	static CURSES_FORCEINLINE char_t top_left() {
		static char_t rlt = CRS_SINLE_TOP_LEFT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t top_right() {
		static char_t rlt = CRS_SINLE_TOP_RIGHT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t bottom_left() {
		static char_t rlt = CRS_SINLE_BOTTOM_LEFT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t bottom_right() {
		static char_t rlt = CRS_SINLE_BOTTOM_RIGHT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t horizontal() {
		static char_t rlt = CRS_SINLE_HORIZONTAL;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t vertical() {
		static char_t rlt = CRS_SINLE_VERTICAL;
		return rlt;
	}
};

struct double_line_corners {
private:
	double_line_corners() {}
public:
	static CURSES_FORCEINLINE char_t top_left() {
		static char_t rlt = CRS_DOUBLE_TOP_LEFT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t top_right() {
		static char_t rlt = CRS_DOUBLE_TOP_RIGHT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t bottom_left() {
		static char_t rlt = CRS_DOUBLE_BOTTOM_LEFT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t bottom_right() {
		static char_t rlt = CRS_DOUBLE_BOTTOM_RIGHT;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t horizontal() {
		static char_t rlt =  CRS_DOUBLE_HORIZONTAL;
		return rlt;
	}
	static CURSES_FORCEINLINE char_t vertical() {
		static char_t rlt = CRS_DOUBLE_VERTICAL;
		return rlt;
	}
};

template<class corners>
class generic_border {
public:
	void out(const pen* p,const bounds& bds,const text_color& color) {
		p->set_color(color);
		// top line
		p->outch(0, 0, corners::top_left());
		hor_line(p, position(1,0), bds.width-1);
		p->outch(bds.width-1, 0, corners::top_right() );
		// bottom line
		p->outch(0, bds.height-1, corners::bottom_left());
		hor_line(p, position(1,bds.height-1), bds.width-1);
		p->outch(bds.width-1, bds.height-1, corners::bottom_right());
		// left line
		vert_line(p, position(0,1), bds.height-1);
		// right line
		vert_line(p, position(bds.width-1,1), bds.height-1);
	}
private:
	inline void hor_line(const pen* p, const position& start, uint8_t width)
	{
		p->out_line(start.x, start.y, width, corners::horizontal() );
	}
	inline void vert_line(const pen* p, const position& start, uint8_t height) {
		for(uint8_t y = start.y; y < height; y++) {
			p->outch(start.x, y, corners::vertical() );
		}
	}
};

} // namesapce detail

//border
border::border(const text_color& color):
	color_(color)
{}

border::~border() CURSES_NOEXCEPT
{}

// single_line_border
single_line_border::single_line_border(const text_color& c):
	border(c)
{}

void single_line_border::out(const pen* p, const bounds& bds)
{
	static detail::generic_border<detail::single_line_corners> impl;
	impl.out(p,bds,color());
}

// double_line_border
double_line_border::double_line_border(const text_color& c):
	border(c)
{}

void double_line_border::out(const pen* p, const bounds& bds)
{
	static detail::generic_border<detail::double_line_corners> impl;
	impl.out(p,bds,color());
}

} // namespace curses
