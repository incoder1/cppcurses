#define WINVER 0x0502
#define _WIN32_WINNT 0x0502
#define _WIN32_WINDOWS 0x0502
// #define WIN32_LEAN_AND_MEAN

#include <cstdio>
#include <application.hpp>
#include <action.hpp>
#include <component.hpp>

using namespace curses;

class TestTexject:public component {
private:
	sp_view create_vew(sp_pen p, rectangle&& r) {
		text_color brdclr(BRIGHT_WHITE,NAVY_BLUE);
		text_color bodyclr(NAVY_YELLOW,NAVY_BLUE);
		sp_border brd(new double_line_titled_border(brdclr,TEXT("Test title")));
		return sp_view(new bordered_box_view(p,CURSES_MOVE(rectangle,r), bodyclr, brd) );
	}
public:
	TestTexject(sp_pen p, const rectangle& rect):
		component( create_vew(p, CURSES_MOVE(rectangle,rect) ) )
	{
		text_color clr( NAVY_YELLOW,  p->current_color().background );
		p->set_color(clr);
	}
	void onKeyPressed(const event& ev) {
		key_state ks = ev.key;
		char_t kcd = ks.key_code;
		if( kcd == (wchar_t) 0x3 ) {
			std::exit(0);
		}
		sp_pen p = view()->get_pen();
		p->set_cursor(0,0);
		p->outch( 0,0, kcd );
		p->out_text(2, 0,TEXT("typed") );
	}
	void onMouse(const event& ev) {
		sp_pen p = view()->get_pen();
		position pos = p->screen_to_virtual( position(ev.mouse.x, ev.mouse.y) );
		wchar_t str[64];
		std::swprintf(str, L"mouse x = %i mouse y = %i", pos.x, pos.y);
		p->out_text( 0, 1, str);
	}
	virtual ~TestTexject()
	{}
};

CURSES_DECLARE_SPTR(TestTexject);

#ifdef CURSES_NO_EXCEPTIONS
namespace boost {
	void throw_exception(std::exception const& exc) {
		std::fprintf( stderr,  exc.what() );
		std::exit(-1);
	}
}
#endif // CURSES_NO_EXCEPTIONS

//int main(int argc, const char** argv)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	sp_terminal trm(new terminal());

#ifdef CURSES_TERMINAL_EMULATOR
    trm->set_window_title(L"CPP Curses test application");
#endif


	bounds bds = trm->get_bounds();

	sp_pen p( new pen( trm.get(), rectangle( 0, 0, bds.width, bds.height) ) );

	application app( p );
	app.run();
	rectangle rect(1, 2, (bds.width-1) / 2, (bds.height-2) / 2 );
    sp_TestTexject tj(new TestTexject(p, rect) );
    tj->show();

	curses::signal ksig( new action( bind_handler(&TestTexject::onKeyPressed, tj) ) );
	ksig.connect(event_type::KEY);

	curses::signal msig(new action( bind_handler(&TestTexject::onMouse, tj ) ) );
	ksig.connect(event_type::MOUSE);

	bool active = true;
	while(active) {
		event ev = trm->wait_for_event();
		switch(ev.type) {
			case event_type::KEY:
					ksig.emit( ev );
					break;
			case event_type::MOUSE:
					msig.emit( ev );
					break;
			default:
				break;
		}
	}
	return 0;
}
