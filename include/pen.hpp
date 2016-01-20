#ifndef __CURSES_PEN_HPP_INCLUDED__
#define __CURSES_PEN_HPP_INCLUDED__

#include "object.hpp"
#include "terminal.hpp"
#include "region.hpp"

namespace curses {

class pen;
CURSES_DECLARE_SPTR(pen);

class CURSES_PUBLIC pen CURSES_FINAL:public object {
public:
	/**
	 * Constructs new pen object
	 * \param trm a tty operations wrapper
	 * \param limit a screen window to output character
	 */
	pen(const terminal* trm, const rectangle& window);
	/**
	 * default virtual destructor
	 */
	virtual ~pen() CURSES_NOEXCEPT;

	/**
	 * Output single character onto terminal
	 * \param x x cood reletive to this pen window coord system
	 * \param y y cood reletive to this pen window coord system
	 * \param ch character to output
	 */
	void outch(uint8_t x, uint8_t y,char_t ch) const;
	/**
	 * Output line of the same character onto terminal
	 * \param x x cood reletive to this pen window coord system
	 * \param y y cood reletive to this pen window coord system
	 * \param w count of characters
	 * \param ch character wich fills the line
	 */
	void out_line(uint8_t x, uint8_t y, uint8_t w, char_t ch) const;
	/**
	 * Output box of characters onto terminal
	 * \param x x cood reletive to this pen window coord system
	 * \param y y cood reletive to this pen window coord system
	 * \param w box width
	 * \param h box height
	 * \param ch character wich fills the box
	 */
	void out_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, char_t ch) const;
	/**
	 * Output a string onto terminal
	 * \param x x cood reletive to this pen window coord system
	 * \param y y cood reletive to this pen window coord system
	 * \param w box width
	 * \param h box height
	 * \param ch character wich fills the box
	 */
	uint8_t out_text(uint8_t x, uint8_t y, const char_t* str) const;
	/**
	 * Sets current terminal text attributes
	 * \param cl terminal text attibutes, i.e. foreground and background colors
	 */
	void set_color(const text_color& cl) const;
	/**
	 * Copy region of terminal screen buffer character into memory
	 * \param x x cood reletive to this pen window coord system
	 * \param y y cood reletive to this pen window coord system
	 * \param w box width
	 * \param h box height
	 * \return copyed data, use paster rect to free allocated resources, alternativelly use std::free avoid delete[]
	 */
	texel* clipt_rect(uint8_t x, uint8_t y, uint8_t &w, uint8_t &h) const;
	void paste_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, texel* const box) const;
	sp_pen sub_pen(const rectangle& rect) const;

	inline sp_pen sub_pen(uint8_t x, uint8_t y, uint8_t w, uint8_t h) const {
		return sub_pen( rectangle( x, y, w, h) );
	}

	inline texel* clipt_rect(const rectangle& rect) const {
		bounds bds = rect.get_bounds();
		return clipt_rect(rect.left(),rect.top(),bds.width,bds.height);
	}

	inline void paste_rect(const rectangle& to, texel* const box) const
	{
		bounds bds = to.get_bounds();
		paste_rect( to.left(), to.top(), bds.width, bds.height, box );
	}

	inline bounds get_bounds() const {
		return relative_.get_bounds();
	}

	inline position get_relative_pos() const {
		return relative_.left_top();
	}

	inline rectangle get_relative() const {
		return relative_;
	}
	inline position screen_to_virtual(const position& pos) const {
		return region_.parent_to_current(pos);
	}
private:
	const terminal* term_;
	region region_;
	rectangle relative_;
};

} // namesapce curses

#endif // __CURSES_PEN_HPP_INCLUDED__
