#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef __WINDOWS_TERMINAL_HPP_INCLUDED__
#define __WINDOWS_TERMINAL_HPP_INCLUDED__

#include <cassert>
#include <cwchar>

#include "basic_terminal_spec.hpp"

namespace curses {

typedef basic_texel<::WORD> texel;
typedef basic_text_color<uint8_t> text_color;

class CURSES_PUBLIC terminal:public basic_terminal<texel, text_color, terminal> {
public:
    explicit terminal();
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
private:
    inline ::WORD current_attributes() const;
private:
    ::HANDLE hStdOut_;
    ::HANDLE hStdIn_;
    ::DWORD ocm_;
    ::HANDLE hCons_;
};

CURSES_DECLARE_SPTR(terminal);

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


} // namesapce curses

#endif // __WINDOWS_TERMINAL_HPP_INCLUDED__
