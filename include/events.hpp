#ifndef __CURSES_EVENTS_HPP_INCLUDED__
#define __CURSES_EVENTS_HPP_INCLUDED__

#include "config.hpp"

#ifdef CURSES_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace curses {

enum event_type: uint8_t
{
	KEY = 0, MOUSE = 1, RESIZE = 2
};

typedef struct _control_key_state {
	bool  capslock:   1;
	bool  enhanced:   1;
	bool  left_alt:   1;
	bool  left_ctrl:  1;
	bool  numlock:    1;
	bool  right_alt:  1;
	bool  right_ctrl: 1;
	bool  scrolllock: 1;
	bool  shift:      8;
} control_key_state;

struct key_state
{
	char_t key_code;
	bool is_down;
	uint8_t repeats;
	control_key_state controls;
};

struct mouse_state
{
	uint8_t x;
	uint8_t y;
	struct
	{
		bool first_btn   : 1;
		bool second_btn  : 1;
		bool third_btn   : 1;
		bool fourth_btn  : 1;
		bool double_click: 1;
		bool hor_wheeled : 1;
		bool vert_wheeled: 1;
		bool moved       : 1;
	} state;
	control_key_state controls;
};

struct resize_state {
	uint8_t width;
	uint8_t height;
};

struct event {
	event_type type;
	union {
		key_state key;
		mouse_state mouse;
		resize_state resize;
	};
};

} // namespace curses


#endif // __CURSES_EVENTS_HPP_INCLUDED__
