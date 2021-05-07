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

	u32 cursor_prev_scr_x = 0;
	u32 cursor_prev_scr_y = 0;
	
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
			struct string* currentln = &e.buf->data[e.buf->cursor.y];  // Current line.
			//struct string* prevln    = &e.buf->data[abs_prev_cur_y];   // Previous line.


			sprintf(titlebar_text, "%s | %i,%i", modes_str[e.buf->mode], e.buf->cursor.x, e.buf->cursor.y);
			draw_text_with_width_map(titlebar_text, strlen(titlebar_text), 1, titlebar_y,
						titlebar_fg_color, titlebar_bg_color, titlebar_bg_color);


			if(e.buf->flags & BUFFER_REDRAW_TEXT) {
				for(u32 i = 0; i < e.buf->height-1; i++) {
					struct string* str = &e.buf->data[i];
					draw_text_with_width_map(str->data, str->len, 0, i, fg_color, bg_color, bg_color);
				}
				e.buf->flags &= ~BUFFER_REDRAW_TEXT;
			}
			else if(e.buf->flags & BUFFER_REDRAW_LINE) {
				draw_text_with_width_map(currentln->data, currentln->len, 0, e.buf->cursor.y, fg_color, bg_color, bg_color);
				e.buf->flags &= ~BUFFER_REDRAW_LINE;
			}

			if(e.buf->flags & BUFFER_REDRAW_CURSOR) {

				struct plx_fb* fb = get_fb();
				const u32 fw = get_font_width();
				const u32 fh = get_font_height();
				const u32 tab_count = string_num_chars(currentln, 0, e.buf->cursor.x+1, '\t');
				const u32 cur_off = fw*(get_font()->tab_width * tab_count - tab_count);

				u32 x = e.buf->cursor.x * fw + cur_off;
				u32 y = e.buf->cursor.y * fh;

				plx_clear_region(fb, cursor_prev_scr_x, cursor_prev_scr_y + fh - 2, fw, 3);

				fb->draw_color = cursor_bg_color;
				plx_draw_region(fb, x, y + fh - 2, fw, 3);

				cursor_prev_scr_x = x;
				cursor_prev_scr_y = y;

				/*
				const u32 tab_count = string_num_chars(currentln, 0, e.buf->cursor.x, '\t');
				const u32 prev_tab_count = string_num_chars(prevln, 0, abs_prev_cur_x, '\t');
				const u32 cur_off = get_font()->tab_width * tab_count - tab_count;
				const u32 prev_cur_off = get_font()->tab_width * prev_tab_count - prev_tab_count;

				char current_char = (e.buf->cursor.x >= currentln->len) ? '<' : currentln->data[e.buf->cursor.x];
				char previous_char = prevln->data[abs_prev_cur_x];
			
				// Clear previous position
				if(abs_prev_cur_x >= prevln->len) {
					draw_rect(abs_prev_cur_x + prev_cur_off, abs_prev_cur_y, 1, 1, bg_color);
				}
				else {
					draw_char(previous_char, abs_prev_cur_x + prev_cur_off, abs_prev_cur_y, fg_color, bg_color);
				}
				
				draw_char(current_char, e.buf->cursor.x + cur_off, e.buf->cursor.y, cursor_fg_color, cursor_bg_color);
				*/
			}

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

