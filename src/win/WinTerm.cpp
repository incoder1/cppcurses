#include "../stdafx.hpp"

#include "WinTerm.hpp"

#ifndef MOUSE_WHEELED
static const ::DWORD MOUSE_WHEELED = 4;
#endif // MOUSE_WHEELED

#ifndef MOUSE_HWHEELED
static const ::DWORD MOUSE_HWHEELED = 8;
#endif // MOUSE_HWHEELED

namespace curses {

CURSES_FORCEINLINE ::WORD color_to_attr(const text_color& color)
{
	return  (color.background << 4) | color.foreground;
}

CURSES_FORCEINLINE text_color attrs_to_color(::WORD attrs)
{
	uint8_t foreground = attrs & 0x000F;
	uint8_t background = attrs >> 12;
	return text_color( foreground, background);
}

static bool resize_console(::HANDLE hConsole, ::SHORT width, ::SHORT height )
{
	::CONSOLE_SCREEN_BUFFER_INFO csbi; // Hold Current Console Buffer Info
	::BOOL bSuccess;
	::SMALL_RECT srWindowRect;// Hold the New Console Size
	::COORD coordScreen;

	bSuccess = ::GetConsoleScreenBufferInfo( hConsole, &csbi );

	// Get the Largest Size we can size the Console Window to
	coordScreen = ::GetLargestConsoleWindowSize( hConsole );

	// Define new possible console size, if requested larger then maximal set it into maximal
	coordScreen.X = std::min(width, coordScreen.X);
	coordScreen.Y = std::min(height, coordScreen.X);

	// Define the New Console Window Size and Scroll Position
	srWindowRect.Left = 0;
	srWindowRect.Top = 0;
	srWindowRect.Right  = coordScreen.X - 1;
	srWindowRect.Bottom = coordScreen.Y - 1;


	// If the Current Buffer is Larger than what we want, Resize the
	// Console Window First, then the Buffer
	if( (csbi.dwSize.X * csbi.dwSize.Y) > (width * height) ) {
		bSuccess = ::SetConsoleWindowInfo( hConsole, TRUE, &srWindowRect );
		bSuccess = ::SetConsoleScreenBufferSize( hConsole, coordScreen );
	}

	// If the Current Buffer is Smaller than what we want, Resize the
	// Buffer First, then the Console Window
	if( (csbi.dwSize.X * csbi.dwSize.Y) < (width * height) ) {
		bSuccess = ::SetConsoleScreenBufferSize( hConsole, coordScreen );
		bSuccess = ::SetConsoleWindowInfo( hConsole, TRUE, &srWindowRect );
	}

	return bSuccess;
}


terminal::terminal():
	hStdOut_(INVALID_HANDLE_VALUE),
	hStdIn_(INVALID_HANDLE_VALUE),
	hCons_(INVALID_HANDLE_VALUE)
{
	hStdOut_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
	assert(INVALID_HANDLE_VALUE != hStdOut_);

	hCons_ = ::CreateConsoleScreenBuffer(
	        GENERIC_READ | GENERIC_WRITE,// read/write access
	        FILE_SHARE_READ | FILE_SHARE_WRITE,// shared
	        NULL,// default security attributes
	        CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
	        NULL);
	assert(INVALID_HANDLE_VALUE != hCons_);

	::CONSOLE_SCREEN_BUFFER_INFO stdInfo;
	::GetConsoleScreenBufferInfo(hStdOut_,&stdInfo);

	if( !resize_console(hCons_, 80, 43) ) {
		std::wprintf(L"Warn! Can not initialize console window. Windows error %i\n", ::GetLastError() );
	}

	::CONSOLE_FONT_INFOEX finfo;
	finfo.cbSize = sizeof(::CONSOLE_FONT_INFOEX);
	::GetCurrentConsoleFontEx(hCons_,TRUE,&finfo);
	finfo.nFont = 2;
	std::swprintf(finfo.FaceName, L"Raster Fonts");
	finfo.dwFontSize.X = 12;
	finfo.dwFontSize.Y = 16;
	finfo.FontFamily = TMPF_DEVICE;
	if(!::SetCurrentConsoleFontEx(hCons_, TRUE, &finfo) ) {
		std::wprintf(L"Warn! Can not initialize console font. Windows error %i\n", ::GetLastError() );
	}

	hStdIn_ = ::GetStdHandle(STD_INPUT_HANDLE);
	assert(INVALID_HANDLE_VALUE != hStdIn_);
	::GetConsoleMode(hStdIn_, &ocm_);

	::DWORD mode = ocm_;
	mode &= ~ENABLE_QUICK_EDIT_MODE; // disable system buffer from mouse selection
	mode &= ~ENABLE_PROCESSED_INPUT; // disable Ctrl+C system event
	mode = mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS; // enable mouse
	assert(::SetConsoleMode(hStdIn_, mode) );

	assert(::SetConsoleActiveScreenBuffer(hCons_));
	::SetConsoleCP(1200); // UTF16LE
}

terminal::~terminal() CURSES_NOEXCEPT {
	assert( ::SetConsoleMode(hStdIn_, ocm_) );
	assert( ::SetConsoleActiveScreenBuffer(hStdOut_) );
	::CloseHandle(hCons_);
}

inline ::WORD terminal::current_attributes() const
{
	::CONSOLE_SCREEN_BUFFER_INFO info;
	::GetConsoleScreenBufferInfo(hCons_,&info);
	return info.wAttributes;
}

bounds terminal::get_bounds() const
{
	::CONSOLE_SCREEN_BUFFER_INFO info;
	::GetConsoleScreenBufferInfo(hCons_,&info);
	return bounds(info.srWindow.Right+1,info.srWindow.Bottom+1);
}

void terminal::set_size(uint8_t width, uint8_t height) const
{
	resize_console(hCons_,width,height);
}

void terminal::putch(const position& pos,char_t ch) const
{
	put_line(pos,ch,1);
}

void terminal::put_line(const position& pos, char_t ch, uint8_t count) const
{
	::COORD coord;
	coord.X = pos.x;
	coord.Y = pos.y;
	::DWORD written = 0;
	::FillConsoleOutputCharacter(hCons_, ch, count, coord, &written);
	assert(written);
	::WORD attrs = current_attributes();
	::FillConsoleOutputAttribute(hCons_, attrs, count, coord, &written);
	assert(written);
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
	assert(::SetConsoleTextAttribute(hCons_, color_to_attr(color)));
}

text_color terminal::current_color() const
{
	return attrs_to_color(current_attributes());
}


void terminal::clipt_rect(const rectangle& rect,texel_type* buff) const
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

void terminal::paste_rect(const rectangle& rect, texel_type* data) const
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

static CURSES_FORCEINLINE bool check_bit(::DWORD val, ::DWORD mask)
{
	return (val & mask) == mask;
}

static CURSES_FORCEINLINE control_key_state extact_controls(::DWORD val)
{
	control_key_state result;
	result.capslock = check_bit(val,CAPSLOCK_ON);
	result.enhanced = check_bit(val,ENHANCED_KEY);
	result.left_alt = check_bit(val,LEFT_ALT_PRESSED);
	result.left_ctrl = check_bit(val,LEFT_CTRL_PRESSED);
	result.numlock = check_bit(val,NUMLOCK_ON);
	result.right_alt = check_bit(val,RIGHT_ALT_PRESSED);
	result.right_ctrl = check_bit(val,RIGHT_CTRL_PRESSED);
	result.scrolllock = check_bit(val,SCROLLLOCK_ON);
	result.shift = check_bit(val,SHIFT_PRESSED);
	return result;
}

static CURSES_FORCEINLINE key_state create_key_event(const ::KEY_EVENT_RECORD& rcd)
{
	key_state result;
	result.key_code = rcd.uChar.UnicodeChar;
	result.is_down = rcd.bKeyDown != FALSE;
	result.repeats = static_cast<uint8_t>(rcd.wRepeatCount);
	result.controls = extact_controls(rcd.dwControlKeyState);
	return result;
}

static CURSES_FORCEINLINE mouse_state create_mouse_event(const ::MOUSE_EVENT_RECORD& rcd)
{
	mouse_state result;
	result.x = rcd.dwMousePosition.X;
	result.y = rcd.dwMousePosition.Y;
	result.state.first_btn   = check_bit(rcd.dwButtonState,FROM_LEFT_1ST_BUTTON_PRESSED);
	result.state.second_btn  = check_bit(rcd.dwButtonState,FROM_LEFT_2ND_BUTTON_PRESSED);
	result.state.third_btn   = check_bit(rcd.dwButtonState,FROM_LEFT_3RD_BUTTON_PRESSED);
	result.state.fourth_btn  = check_bit(rcd.dwButtonState,FROM_LEFT_4TH_BUTTON_PRESSED);
	result.state.double_click = check_bit(rcd.dwEventFlags,DOUBLE_CLICK);
	result.state.hor_wheeled  = check_bit(rcd.dwEventFlags,MOUSE_HWHEELED);
	result.state.vert_wheeled = check_bit(rcd.dwEventFlags,MOUSE_WHEELED);
	result.state.moved = check_bit(rcd.dwEventFlags,MOUSE_MOVED);
	result.controls = extact_controls(rcd.dwControlKeyState);
	return result;
}

static CURSES_FORCEINLINE resize_state create_resize_event(const ::WINDOW_BUFFER_SIZE_RECORD& rcd)
{
	resize_state result;
	result.width = rcd.dwSize.X;
	result.height = rcd.dwSize.Y;
	return result;
}

event terminal::wait_for_event() const
{
	static ::INPUT_RECORD irb[1];
	::DWORD read = 0;
	event result;
	while( !read  ) {
		::ReadConsoleInput(hStdIn_, irb, 1, &read);
		switch(irb[0].EventType) {
		case KEY_EVENT:
			result.type = event_type::KEY;
			result.key =  create_key_event(irb[0].Event.KeyEvent);
			break;
		case MOUSE_EVENT:
			result.type = event_type::MOUSE;
			result.mouse = create_mouse_event(irb[0].Event.MouseEvent);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			result.type = event_type::RESIZE;
			result.resize = create_resize_event(irb[0].Event.WindowBufferSizeEvent);
			break;
		}
	}
	return result;
}

} // namespace curses
