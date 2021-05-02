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


/*
void testing_stuff_delete_later() {
	lua_State* L = luaL_newstate();

	if(L == NULL) {
		printf("lua state is NULL!\n");
	}
	printf("%p\n", L);

	const char* test = "a = 1 + 2";
	int result = luaL_dostring(L, test);
	if(result == 0) {
		printf("ok\n");
	}
	else {
		printf("no\n");
	}

	lua_getglobal(L, "a");
	if(lua_isnumber(L, -1)) {
		printf("a = %f\n", lua_tonumber(L, -1));
	}

	lua_close(L);
}
*/

void draw_frame(struct frame* fr, struct plx_font* font) {

	plx_color(100, 100, 100);
	plx_draw_rect(fr->x, fr->y, fr->width, fr->height);

	plx_color(255, 255, 255);
	plx_draw_text(fr->x, fr->y, fr->title.data, fr->title.len, font);

}



int main(int argc, char** argv) {

	plx_init();
	if(plx_getstatus() & PLX_ERR) {
		plx_exit();
		return -1;
	}

	struct plx_font font;
	struct edi e;
	init_program(&e);

	e.lua_state = luaL_newstate();
	if(e.lua_state == NULL) {
		plx_exit();
		fprintf(stderr, "Failed to initialize lua!\n");
		return -1;
	}

	plx_load_font(FONT_FILE, &font);
    font.scale = FONT_SCALE;
	font.spacing = FONT_SPACING;
	font.tabwidth = TAB_WIDTH;

	u32 width = 0;
	u32 height = 0;

	plx_getres(&width, &height);
	plx_clear_color(10, 10, 83);
	plx_swap_buffers();


	char title[64];
	char* modes[5] = {
		"MODE_INVALID",
		"MODE_INSERT",
		"MODE_REPLACE",
		"MODE_SELECT",
		"MODE_CMD"
	};

	// TODO: add font screen dimensions to framebuffer utils.
	const u32 font_scr_width = (font.header.width + font.spacing) * font.scale;
	const u32 font_scr_height = (font.header.height + font.spacing) * font.scale;
	const u32 title_y = TITLE_POS ? 0 : height - font_scr_height;
	const u32 max_cols = (width / font_scr_width) - 1;
	const u32 max_rows = (height / font_scr_height) - 1;

	add_buffer(&e, max_cols, max_rows);

	if(argc > 1) {
		open_file(argv[1], e.buf);
	}

	// TODO: optimization after everything i need is working.
	// - try to keep redrawing to minumum
	// - dont draw stuff that are not visible.
	// ...

    while(1) {
		plx_delay(5);
	
		// Title bar
		plx_color(80, 80, 80);
		plx_draw_rect(0, title_y, width, font_scr_height);


		if(e.buf != NULL) {
			struct string* currentln = &e.buf->data[e.buf->cursor_y];
			
			// Cursor
			plx_color(80, 200, 80);
			const u32 cur_off = string_num_chars(currentln, 0, e.buf->cursor_x+1, '\t');  // Count tab characters from start of line to cursor x position.
			const u32 cur_scr_x = (e.buf->cursor_x + (cur_off * font.tabwidth) - cur_off) * font_scr_width;
			
			if(e.buf->mode != COMMAND_INPUT) {
				plx_draw_rect(cur_scr_x, (e.buf->cursor_y + TITLE_POS) * font_scr_height, font_scr_width, font_scr_height);
			}
			else {
				plx_draw_rect(cur_scr_x + TITLE_OFFSET, title_y, font_scr_width, font_scr_height);
			}

			// Title text
			plx_color(25, 255, 255);
			if(e.buf->mode != COMMAND_INPUT) {
				sprintf(title, "%s | %ix%i | %i,%i", modes[e.buf->mode], e.buf->width, e.buf->height, e.buf->cursor_x, e.buf->cursor_y);
				plx_draw_text(TITLE_OFFSET, title_y, title, strlen(title), &font);
			}
			else {
				plx_draw_text(TITLE_OFFSET, title_y, e.buf->cmd_input.data, e.buf->cmd_input.len, &font);
			}
		
			// Text
			// (TODO: dont use time to render stuff that is actually not visible at the moment.)
			plx_color(255, 255, 255);
			for(u32 i = 0; i < e.buf->last_line; i++) {
				const struct string* str = &e.buf->data[i];
				plx_draw_text(1, (i + TITLE_POS) * font_scr_height, str->data, MIN(e.buf->width, str->len), &font);
			}

			// Frames
			for(u32 i = 0; i < e.frame_count; i++) {
				draw_frame(&e.frames[i], &font);
			}

			plx_swap_buffers();
			if(e.buf->mode != COMMAND_INPUT) {
				handle_input(&e);
			}
			else {
				handle_command_input(&e);
			}

		}
		else {
			plx_color(25, 255, 255);
			plx_draw_text(TITLE_OFFSET, title_y, "NULL", 4, &font);
			
			plx_swap_buffers();
			handle_input(&e);
		}
    }

    plx_unload_font(&font);
    plx_exit();
}

