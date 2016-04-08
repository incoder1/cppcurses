#ifndef __CURSES_VESATERM_HPP_INCLUDED__
#define __CURSES_VESATERM_HPP_INCLUDED__

#include "basic_terminal_spec.hpp"

#define CRS_SINLE_TOP_LEFT  0xDA
#define CRS_SINLE_TOP_RIGHT 0xBF
#define CRS_SINLE_BOTTOM_LEFT 0xC0
#define CRS_SINLE_BOTTOM_RIGHT 0xD9
#define CRS_SINLE_HORIZONTAL 0xC4
#define CRS_SINLE_VERTICAL 0xB3

#define CRS_DOUBLE_TOP_LEFT  0xC9
#define CRS_DOUBLE_TOP_RIGHT 0xBB
#define CRS_DOUBLE_BOTTOM_LEFT OxC8
#define CRS_DOUBLE_BOTTOM_RIGHT 0xBC
#define CRS_DOUBLE_HORIZONTAL 0xCD
#define CRS_DOUBLE_VERTICAL 0xBA

namespace curses {

typedef basic_texel<uint8_t> texel;
typedef basic_text_color<uint8_t> text_color;

enum color: uint8_t {
    BLACK        = 0x0,
    NAVY_BLUE    = 0x1,
    NAVY_GREEN   = 0x2,
    NAVY_AQUA    = 0x3,
    NAVY_RED     = 0x4,
    NAVY_PURPLE  = 0x5,
    NAVY_YELLOW  = 0x6,
    WHITE        = 0x7,
    GREY         = 0x8,
    LIGHT_BLUE   = 0x9,
    LIGHT_GREEN  = 0xA,
    LIGHT_AQUA   = 0xB,
    LIGHT_RED    = 0xC,
    LIGHT_PURPLE = 0xD,
    LIGHT_YELLOW = 0xE,
    BRIGHT_WHITE = 0xF
};

enum vesa_mode
{
	MODE_132_60;
};

class terminal:public basic_terminal<texel, text_color, terminal>
{
public:
	explicit terminal(vesa_mode mode);
    virtual ~terminal() CURSES_NOEXCEPT;
	bounds get_bounds() const;
    void set_size(uint8_t width, uint8_t height) const;
    void putch(const position& pos,char_t ch) const;
    void put_line(const position& pos,char_t ch, uint8_t count) const;
    uint8_t put_text(const position& pos, const char_t* str) const;
    void fill_rectangle(const rectangle& r,char_t ch) const;
    void set_color(const text_color& color) const;
    text_color current_color() const;
    void clipt_rect(const rectangle& rect,texel_type* buff) const;
    void paste_rect(const rectangle& rect, texel_type* data) const;
    event wait_for_event() const;
public:
	bounds size_;
	uint8_t currentAttrs_;
	static texel ** _vbuff;
};

CURSES_DECLARE_SPTR(terminal);

} // namespace curses


#endif // __CURSES_VESATERM_HPP_INCLUDED__
