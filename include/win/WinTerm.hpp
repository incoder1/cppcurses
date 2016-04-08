#ifndef __WINDOWS_TERMINAL_HPP_INCLUDED__
#define __WINDOWS_TERMINAL_HPP_INCLUDED__

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <cassert>
#include <cwchar>
#include <wincon.h>

#include "basic_terminal_spec.hpp"

// undocument Win API console functions from kernel32,
// requared for normal work with the console
extern "C" {

	BOOL WINAPI SetConsoleFont(::HANDLE hOutput, ::DWORD fontIndex);

	BOOL WINAPI SetConsoleIcon(::HICON hIcon);

}

//static const ::DWORD SCRIPT_UNICODE_FONT = 11;

namespace curses {

typedef basic_texel<::WORD> texel;
typedef basic_text_color<uint8_t> text_color;

class CURSES_PUBLIC terminal:public object {
public:
    explicit terminal();
    virtual ~terminal() CURSES_NOEXCEPT;
    bounds get_bounds() const;
    void move_cursor(uint8_t x, uint8_t y) const;
	void set_cursor_visibility(bool visible) const;
    void set_size(uint8_t width, uint8_t height) const;
    void putch(const position& pos,char_t ch) const;
    void put_line(const position& pos,char_t ch, uint8_t count) const;
    uint8_t put_text(const position& pos, const char_t* str) const;
    void fill_rectangle(const rectangle& r,char_t ch) const;
    void set_color(const text_color& color) const;
    text_color current_color() const;
    void clipt_rect(const rectangle& rect,texel* buff) const;
    void paste_rect(const rectangle& rect,texel* data) const;
    event wait_for_event() const;
    void set_window_title(const char_t* title) const;
private:
    inline ::WORD current_attributes() const;
    static CURSES_FORCEINLINE bool check_bit(::DWORD val, ::DWORD mask)
    {
		return (val & mask) == mask;
    }
    static control_key_state extact_controls(::DWORD val);
    static event create_key_event(const ::KEY_EVENT_RECORD& rcd);
    static event create_mouse_event(const ::MOUSE_EVENT_RECORD& rcd);
    static event create_resize_event(const ::WINDOW_BUFFER_SIZE_RECORD& rcd);
    static void init_colors(::HANDLE hcons);
private:
    ::HANDLE hStdOut_;
    ::HANDLE hStdIn_;
    ::DWORD ocm_;
    ::HANDLE hCons_;
    bool extConsole_;
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
