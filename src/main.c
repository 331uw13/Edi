#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "plx.h"
#include "buffer.h"
#include "file_io.h"
#include "input_handler.h"
#include "util.h"
#include "config.h" // <---  settings are here!  (TODO: read values from lua file)

#include "draw.h"



int main(int argc, char** argv) {

	struct edi e;
	init_program(&e);

	e.lua_state = luaL_newstate();
	if(e.lua_state == NULL) {
		fprintf(stderr, "Failed to initialize lua!\n");
		return -1;
	}
	

	luaL_openlibs(e.lua_state);
	luaL_dofile(e.lua_state, "config");

	u32 bg_color = lua_getint(e.lua_state, "back_color");
	u32 fg_color = lua_getint(e.lua_state, "text_color");
	u32 titlebar_bg_color = lua_getint(e.lua_state, "titlebar_back_color");
	u32 titlebar_fg_color = lua_getint(e.lua_state, "titlebar_text_color");
	u32 cursor_bg_color = lua_getint(e.lua_state, "cursor_back_color");
	u32 cursor_fg_color = lua_getint(e.lua_state, "cursor_text_color");

	init_drawing();
	add_buffer(&e, get_max_col(), get_max_row());
	
	if(argc > 1) {
		open_file(argv[1], e.buf);
	}

	u32 abs_prev_cur_x = 0;
	u32 abs_prev_cur_y = 0;
	
	clear_screen(bg_color);
	
	// Title bar.
	const u32 titlebar_y = get_max_row() - 1;
	draw_rect(0, titlebar_y, get_max_col(), 1, titlebar_bg_color);
	char titlebar_text[64];

	char* modes_str[5] = {
		"MODE_INVALID",
		"MODE_INSERT",
		"MODE_REPLACE",
		"MODE_SELECT",
		"COMMAND_INPUT"
	};


    while(1) {
		plx_delay(10);
		if(e.buf != NULL) {
			struct string* currentln = &e.buf->data[e.buf->cursor.y];


			sprintf(titlebar_text, "%s | %i,%i", modes_str[e.buf->mode], e.buf->cursor.x, e.buf->cursor.y);
			draw_text_with_width_map(titlebar_text, strlen(titlebar_text), 1, titlebar_y,
						titlebar_fg_color, titlebar_bg_color);


			if(e.buf->flags & BUFFER_REDRAW_TEXT) {
				for(u32 i = 0; i < e.buf->height-1; i++) {
					struct string* str = &e.buf->data[i];
					draw_text_with_width_map(str->data, str->len, 0, i, fg_color, bg_color);
				}
				e.buf->flags &= ~BUFFER_REDRAW_TEXT;
			}
			else if(e.buf->flags & BUFFER_REDRAW_LINE) {
				struct string* str = &e.buf->data[e.buf->cursor.y];
				draw_text_with_width_map(str->data, str->len, 0, e.buf->cursor.y, fg_color, bg_color);
				e.buf->flags &= ~BUFFER_REDRAW_LINE;
			}

			if(e.buf->flags & BUFFER_REDRAW_CURSOR) {
				struct string* prevln = &e.buf->data[abs_prev_cur_y];

				if(abs_prev_cur_x >= prevln->len) {
					clear_space(abs_prev_cur_x, abs_prev_cur_y, 1);
				}
				else {
					draw_char(prevln->data[abs_prev_cur_x], abs_prev_cur_x, abs_prev_cur_y, fg_color, bg_color);
				}

				char char_on_cursor = (e.buf->cursor.x >= currentln->len) ? '<' : currentln->data[e.buf->cursor.x];
				draw_char(char_on_cursor, e.buf->cursor.x, e.buf->cursor.y, cursor_fg_color, cursor_bg_color);
			}


			abs_prev_cur_x = e.buf->cursor.x;
			abs_prev_cur_y = e.buf->cursor.y;

			if(e.buf->mode != COMMAND_INPUT) {
				handle_input(&e);
			}
			else {
				handle_command_input(&e);
			}
		}
		else {
			handle_input(&e);
		}
    }
}

