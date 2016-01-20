#ifndef __WINDOWS_CONSOLE_TERMINAL_HPP_INCLUDED__
#define __WINDOWS_CONSOLE_TERMINAL_HPP_INCLUDED__

#if !defined(UNICODE) && !defined(_UNICODE)
#	error Windows Implementation requares Unicode
#endif // unicode check

#include <assert.h>
#include <cstdlib>

#include <windows.h>
#include "../basic_terminal_spec.hpp"

namespace curses {

	enum color: uint8_t {
		BLACK    = 0x0,
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

	typedef basic_texel<::TCHAR,::WORD> texel;
	typedef basic_text_color<uint8_t> text_color;

	namespace win {

	class terminal:public basic_terminal<texel, text_color, terminal>, public virtual object {
		public:
			explicit terminal():
				hStdOut_(INVALID_HANDLE_VALUE),
				hStdIn_(INVALID_HANDLE_VALUE),
				hCons_(INVALID_HANDLE_VALUE)
			{
				hStdOut_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
				assert(INVALID_HANDLE_VALUE != hStdOut_);

				hStdIn_ = ::GetStdHandle(STD_INPUT_HANDLE);
				assert(INVALID_HANDLE_VALUE != hStdIn_);

				::SetConsoleCP(1200); // UTF16LE
				hCons_ = ::CreateConsoleScreenBuffer(
					GENERIC_READ | GENERIC_WRITE,// read/write access
					FILE_SHARE_READ | FILE_SHARE_WRITE,// shared
					NULL,// default security attributes
					CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
					NULL);
				assert(INVALID_HANDLE_VALUE != hCons_);
				::DWORD ec = ::SetConsoleActiveScreenBuffer(hCons_);
				assert(ec);
				assert(::SetConsoleMode(hStdIn_, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT));
			}

			virtual ~terminal() CURSES_NOEXCEPT {
				::DWORD ec = ::SetConsoleActiveScreenBuffer(hStdOut_);
				assert(ec);
				::CloseHandle(hCons_);
			}

			bounds get_bounds() const {
				::CONSOLE_SCREEN_BUFFER_INFO info;
				::GetConsoleScreenBufferInfo(hCons_,&info);
				return bounds(info.srWindow.Right+1,info.srWindow.Bottom+1);
			}

			void set_size(uint8_t width, uint8_t height) const {
				::SMALL_RECT sr;
				sr.Left = 0;
				sr.Top = 0;
				sr.Right = width;
				sr.Bottom = height;
				::SetConsoleWindowInfo(hCons_,TRUE,&sr);
			}

			void putch(const position& pos,char_t ch) const
			{
				put_line(pos,ch,1);
			}

			void put_line(const position& pos,char_t ch, uint8_t count) const
			{
				::DWORD written = 0;
				::COORD coord;
				coord.X = pos.x;
				coord.Y = pos.y;
				::FillConsoleOutputCharacter(hCons_, ch, count, coord, &written);
				assert(written);
				attrs_t attrs = current_attributes();
				::FillConsoleOutputAttribute(hCons_, attrs, count, coord, &written);
				assert(written);
			}

			uint8_t put_text(const position& pos, const char_t* str) const {
				char_t *it = const_cast<char_t*>(str);
				position outp = pos;
				uint8_t result = 0;
				do {
					putch(outp,*it);
					++outp.x;
					++it;
					++result;
				} while(*it != 0);
				return result;
			}

			void fill_rectangle(const rectangle& r,char_t ch) const {
				position pos = r.left_top();
				bounds bds = r.get_bounds();
				uint8_t count = bds.width;
				uint8_t lines = r.left() + bds.height;
				for(uint8_t y = r.top(); y < lines; y++ ) {
					pos.y = y;
					put_line(pos,ch,count);
				}
			}

			attrs_t make_attrs(const text_color& color) const {
				return  (color.background << 4) | color.foreground;
			}

			void set_out_attrs(attrs_t attrs) const {
				::SetConsoleTextAttribute(hCons_, attrs);
			}

			attrs_t current_attributes() const
			{
				::CONSOLE_SCREEN_BUFFER_INFO info;
				::GetConsoleScreenBufferInfo(hCons_,&info);
				return info.wAttributes;
			}

			void clipt_rect(const rectangle& rect,texel_type* buff) const
			{
				::COORD coordBufCoord;
				coordBufCoord.X = 0;
				coordBufCoord.Y = 0;

				bounds bds = rect.get_bounds();
				::COORD coordBufSize;
				coordBufSize.X = bds.width;
				coordBufSize.Y = bds.height;

				::SMALL_RECT sr;
				sr.Left = rect.left();
				sr.Top  = rect.top();
				sr.Right = rect.right();
				sr.Bottom = rect.bottom();

				CHAR_INFO *chiBuffer = reinterpret_cast<CHAR_INFO*>(buff);

				::BOOL fSuccess = ::ReadConsoleOutputW(
					hCons_, // screen buffer to read from
					chiBuffer, // buffer to copy into
					coordBufSize,   // col-row size of chiBuffer
					coordBufCoord,  // top left dest. cell in chiBuffer
					&sr); // screen buffer source rectangle
				assert(fSuccess);
			}

			void paste_rect(const rectangle& rect, texel_type* data) const
			{
				::COORD coordBufCoord;
				coordBufCoord.X = 0;
				coordBufCoord.Y = 0;

				bounds bds = rect.get_bounds();
				::COORD coordBufSize;
				coordBufSize.X = bds.width;
				coordBufSize.Y = bds.height;

				::SMALL_RECT sr;
				sr.Left = rect.left();
				sr.Top  = rect.top();
				sr.Right = rect.right();
				sr.Bottom = rect.bottom();

				::CHAR_INFO *chiBuffer = reinterpret_cast<::CHAR_INFO*>(data);

				::BOOL fSuccess = ::WriteConsoleOutputW(
						hCons_, // screen buffer to write to
						chiBuffer,        // buffer to copy from
						coordBufSize,     // col-row size of chiBuffer
						coordBufCoord,    // top left src cell in chiBuffer
						&sr);  // dest. screen buffer rectangle
				assert(fSuccess);
			}

		private:
			::HANDLE hStdOut_;
			::HANDLE hStdIn_;
			::HANDLE hCons_;
		};

	} // namespace win

	// define the texel and terminal types
	typedef win::terminal terminal;
	CURSES_DECLARE_SPTR(terminal);

} // namesapce curses


#endif // __WINDOWS_CONSOLE_TERMINAL_HPP_INCLUDED__
