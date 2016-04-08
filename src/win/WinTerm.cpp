#include "../stdafx.hpp"

#include "WinTerm.hpp"

#ifndef MOUSE_WHEELED
static const ::DWORD MOUSE_WHEELED = 4;
#endif // MOUSE_WHEELED

#ifndef MOUSE_HWHEELED
static const ::DWORD MOUSE_HWHEELED = 8;
#endif // MOUSE_HWHEELED

static const ::DWORD SCRIPT_MODE_TRUE_TYPE_FONT = 9;
static const ::DWORD BOX_DRAWING_RASTER_FONT = 8;

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

#define WM_SETCONSOLEINFO			(WM_USER+201)

#ifndef CURSES_HAS_CPP11
#pragma pack(push, 1)
typedef struct _CONSOLE_INFO
{
	ULONG		Length;
	COORD		ScreenBufferSize;
	COORD		WindowSize;
	ULONG		WindowPosX;
	ULONG		WindowPosY;

	COORD		FontSize;
	ULONG		FontFamily;
	ULONG		FontWeight;
	WCHAR		FaceName[32];

	ULONG		CursorSize;
	ULONG		FullScreen;
	ULONG		QuickEdit;
	ULONG		AutoPosition;
	ULONG		InsertMode;

	USHORT		ScreenColors;
	USHORT		PopupColors;
	ULONG		HistoryNoDup;
	ULONG		HistoryBufferSize;
	ULONG		NumberOfHistoryBuffers;

	COLORREF	ColorTable[16];

	ULONG		CodePage;
	HWND		Hwnd;

	WCHAR		ConsoleTitle[0x100];
} CONSOLE_INFO;
#pragma pack(pop)
#else
typedef struct alignas(8) _CONSOLE_INFO
{
	ULONG		Length;
	COORD		ScreenBufferSize;
	COORD		WindowSize;
	ULONG		WindowPosX;
	ULONG		WindowPosY;

	COORD		FontSize;
	ULONG		FontFamily;
	ULONG		FontWeight;
	WCHAR		FaceName[32];

	ULONG		CursorSize;
	ULONG		FullScreen;
	ULONG		QuickEdit;
	ULONG		AutoPosition;
	ULONG		InsertMode;

	USHORT		ScreenColors;
	USHORT		PopupColors;
	ULONG		HistoryNoDup;
	ULONG		HistoryBufferSize;
	ULONG		NumberOfHistoryBuffers;

	COLORREF	ColorTable[16];

	ULONG		CodePage;
	HWND		Hwnd;

	WCHAR		ConsoleTitle[0x100];
} CONSOLE_INFO;
#endif // CURSES_HAS_CPP11

BOOL SetConsoleInfo(HWND hwndConsole, CONSOLE_INFO *pci)
{
	DWORD   dwConsoleOwnerPid;
	HANDLE  hProcess;
	HANDLE	hSection, hDupSection;
	PVOID   ptrView = 0;
	HANDLE  hThread;

	//
	//	Open the process which "owns" the console
	//
	::GetWindowThreadProcessId(hwndConsole, &dwConsoleOwnerPid);

	hProcess = ::OpenProcess(MAXIMUM_ALLOWED, FALSE, dwConsoleOwnerPid);

	//
	// Create a SECTION object backed by page-file, then map a view of
	// this section into the owner process so we can write the contents
	// of the CONSOLE_INFO buffer into it
	//
	hSection = ::CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, pci->Length, 0);
	//
	//	Copy our console structure into the section-object
	//
	ptrView = ::MapViewOfFile(hSection, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, pci->Length);

	memcpy(ptrView, pci, pci->Length);

	::UnmapViewOfFile(ptrView);

	//
	//	Map the memory into owner process
	::DuplicateHandle(::GetCurrentProcess(), hSection, hProcess, &hDupSection, 0, FALSE, DUPLICATE_SAME_ACCESS);

	//  Send console window the "update" message
	::SendMessage(hwndConsole, WM_SETCONSOLEINFO, (WPARAM)hDupSection, 0);

	hThread = ::CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)::CloseHandle, hDupSection, 0, 0);

	::CloseHandle(hThread);
	::CloseHandle(hSection);
	::CloseHandle(hProcess);

	return TRUE;
}

static void GetConsoleSizeInfo(::HANDLE hcons, CONSOLE_INFO *pci)
{
	::CONSOLE_SCREEN_BUFFER_INFO csbi;
	::GetConsoleScreenBufferInfo(hcons, &csbi);

	pci->ScreenBufferSize = csbi.dwSize;
	pci->WindowSize.X	  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	pci->WindowSize.Y	  = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	pci->WindowPosX	      = csbi.srWindow.Left;
	pci->WindowPosY		  = csbi.srWindow.Top;
}

static void SetConsolePalette(::HANDLE hcons, const ::COLORREF* palette)
{

	CONSOLE_INFO ci = { sizeof(ci) };
	int i;
        HWND hwndConsole = GetConsoleWindow();
	// get current size/position settings rather than using defaults..
	GetConsoleSizeInfo(hcons, &ci);

	// set these to zero to keep current settings
	ci.FontSize.X				= 0;//8;
	ci.FontSize.Y				= 0;//12;
	ci.FontFamily				= 0;//0x30;//FF_MODERN|FIXED_PITCH;//0x30;
	ci.FontWeight				= 0;//0x400;
	//::lstrcpyW(ci.FaceName, L"Terminal");
	ci.FaceName[0]				= L'\0';

	ci.CursorSize				= 25;
	ci.FullScreen				= FALSE;
	ci.QuickEdit				= TRUE;
	ci.AutoPosition				= 0x10000;
	ci.InsertMode				= TRUE;
	ci.ScreenColors				= MAKEWORD(0x7, 0x0);
	ci.PopupColors				= MAKEWORD(0x5, 0xf);

	ci.HistoryNoDup				= FALSE;
	ci.HistoryBufferSize		= 50;
	ci.NumberOfHistoryBuffers	= 4;
	// colour table
	for(i = 0; i < 16; i++) {
		ci.ColorTable[i] = palette[i];
	}
	ci.CodePage	= 0;//0x352;
	ci.Hwnd = hwndConsole;
	lstrcpyW(ci.ConsoleTitle, L"");
	SetConsoleInfo(hwndConsole, &ci);
}


// Terminal
terminal::terminal():
	hStdOut_(INVALID_HANDLE_VALUE),
	hStdIn_(INVALID_HANDLE_VALUE),
	hCons_(INVALID_HANDLE_VALUE),
	extConsole_(false)
{
	extConsole_ = ::AttachConsole(ATTACH_PARENT_PROCESS);
	// for a gui application try to attach calling console, or allocate a new one
	if( !extConsole_) {
		// check current application is alrady a console one
		if(ERROR_ACCESS_DENIED != ::GetLastError() ) {
			// if gui applicaion with WinMain entry point
			// started from process withot allocated console
			extConsole_ = ::AllocConsole();
			if(!extConsole_) {
				::MessageBoxW(static_cast<::HWND>(NULL),L"Can not initialize console", L"Error", MB_OK | MB_ICONERROR);
				::ExitProcess(-1);
			}
		}
	}

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


	if(!::SetConsoleFont(hCons_, BOX_DRAWING_RASTER_FONT) ) {
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

	init_colors(hCons_);
}

terminal::~terminal() CURSES_NOEXCEPT {
	assert( ::SetConsoleMode(hStdIn_, ocm_) );
	assert( ::SetConsoleActiveScreenBuffer(hStdOut_) );
	::CloseHandle(hCons_);
	if(extConsole_)
	{
		::FreeConsole();
	}
}

void terminal::init_colors(::HANDLE hcons)
{
  ::COLORREF palette[16] =
  {
    0x00000000, 0x00800000, 0x00008000, 0x00808000,
    0x00000080, 0x00800080, 0x00008080, 0x00c0c0c0,
    0x00808080, 0x00ff0000, 0x0000ff00, 0x00ffff00,
    0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff
  };
  SetConsolePalette(hcons, palette);
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

void terminal::move_cursor(uint8_t x, uint8_t y) const
{
	::COORD pos;
	pos.X = x;
	pos.Y = y;
	::SetConsoleCursorPosition(hCons_, pos);
}

void terminal::set_cursor_visibility(bool visible) const
{
	::CONSOLE_CURSOR_INFO  info;
	::GetConsoleCursorInfo(hCons_, &info);
	info.bVisible = visible; // set the cursor visibility
	::SetConsoleCursorInfo(hCons_, &info);
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


void terminal::clipt_rect(const rectangle& rect,texel* buff) const
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

void terminal::paste_rect(const rectangle& rect, texel* data) const
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

void terminal::set_window_title(const char_t* title) const
{
	::SetConsoleTitleW(title);
}

control_key_state terminal::extact_controls(::DWORD val)
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

event terminal::create_key_event(const ::KEY_EVENT_RECORD& rcd)
{
	event result;
	result.type = event_type::KEY;
	result.key.key_code = rcd.uChar.UnicodeChar;
	result.key.is_down = rcd.bKeyDown != FALSE;
	result.key.repeats = static_cast<uint8_t>(rcd.wRepeatCount);
	result.key.controls = extact_controls(rcd.dwControlKeyState);
	return result;
}

event terminal::create_mouse_event(const ::MOUSE_EVENT_RECORD& rcd)
{
	event result;
	result.type = event_type::MOUSE;
	result.mouse.x = rcd.dwMousePosition.X;
	result.mouse.y = rcd.dwMousePosition.Y;
	result.mouse.state.first_btn   = check_bit(rcd.dwButtonState,FROM_LEFT_1ST_BUTTON_PRESSED);
	result.mouse.state.second_btn  = check_bit(rcd.dwButtonState,FROM_LEFT_2ND_BUTTON_PRESSED);
	result.mouse.state.third_btn   = check_bit(rcd.dwButtonState,FROM_LEFT_3RD_BUTTON_PRESSED);
	result.mouse.state.fourth_btn  = check_bit(rcd.dwButtonState,FROM_LEFT_4TH_BUTTON_PRESSED);
	result.mouse.state.double_click = check_bit(rcd.dwEventFlags,DOUBLE_CLICK);
	result.mouse.state.hor_wheeled  = check_bit(rcd.dwEventFlags,MOUSE_HWHEELED);
	result.mouse.state.vert_wheeled = check_bit(rcd.dwEventFlags,MOUSE_WHEELED);
	result.mouse.state.moved = check_bit(rcd.dwEventFlags,MOUSE_MOVED);
	result.mouse.controls = extact_controls(rcd.dwControlKeyState);
	return result;
}

event terminal::create_resize_event(const ::WINDOW_BUFFER_SIZE_RECORD& rcd)
{
	event result;
	result.type = event_type::RESIZE;
	result.resize.width = rcd.dwSize.X;
	result.resize.height = rcd.dwSize.Y;
	return result;
}

event terminal::wait_for_event() const
{
	static ::INPUT_RECORD irb[1];
	::DWORD read = 0;
	while( !read  ) {
		::ReadConsoleInput(hStdIn_, irb, 1, &read);
	}
	switch(irb[0].EventType) {
	case KEY_EVENT:
		return create_key_event(irb[0].Event.KeyEvent);
	case MOUSE_EVENT:
		return create_mouse_event(irb[0].Event.MouseEvent);
	case WINDOW_BUFFER_SIZE_EVENT:
		return create_resize_event(irb[0].Event.WindowBufferSizeEvent);
	default:
		event ev;
		return ev;
	}
}


} // namespace curses
