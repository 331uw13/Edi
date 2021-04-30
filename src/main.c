#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "plx.h"
#include "buffer.h"
#include "cursor.h"
#include "util.h"
#include "config.h" // <--- settings are here!  (TODO: config file)

#define ENTER      0xD
#define BACKSPACE  0x7F
#define TAB        0x9
#define ESC        0x1B


void handle_enter_key(struct edit_buffer* buffer, struct cursor* cur) {
	if(buffer_health_check(buffer) && cur->y < buffer->last_line) {
		struct string* str = &buffer->data[cur->y];
		const u32 x = MIN(cur->x, str->len);
		const u32 y = MIN(cur->y, buffer->last_line);
		
		if(buffer_shift_lines(buffer, BUFFER_SHIFT_DOWN, y)) {
			if(buffer_cut(buffer, str, x, str->len, 0, y+1)) {
				cur->x = 0;
				cur->y++;
			}
		}
	}
}

void handle_backspace_key(struct edit_buffer* buffer, struct cursor* cur) {
	if(buffer_health_check(buffer) && cur->y < buffer->last_line) {
		struct string* str = &buffer->data[cur->y];
		
		if(cur->x == 0 && cur->y > 0) {
			const u32 x = MIN(cur->x, str->len);
			const u32 y = MIN(cur->y, buffer->last_line);

			if(buffer_shift_lines(buffer, BUFFER_SHIFT_UP, y)) {
				cur->y--;
			}
		}
		else {
			if(buffer_remchr(buffer, cur->x, cur->y)) {
				cur->x--;
				cur->prev_line_x = cur->x;
			}
		}
	}
}


void handle_input(struct edit_buffer* buffer, struct cursor* cur) {
	u8 input = plx_keyinput();

	/* DELETE THIS */ char cmd[32];
	switch(input) {
		
		case KEY_QUIT_EDITOR:
			//printf("%x\n", plx_keyinput());
			plx_exit();
			destroy_buffer(buffer);

			/* DELETE THIS */ sprintf(cmd, "ps -p %d -o %%cpu,%%mem > log.txt", getpid());
		 	/* DELETE THIS */ system(cmd);

			exit(0);
			break;

		case KEY_SWAP_LINE_UP:
			if(cur->y > 0) {
				swap_string(&buffer->data[cur->y], &buffer->data[cur->y-1]);
				cur->y--;
			}
			break;
		
		case KEY_SWAP_LINE_DOWN:
			if(cur->y < buffer->last_line) {
				swap_string(&buffer->data[cur->y], &buffer->data[cur->y+1]);
				cur->y++;
			}
			break;

		case KEY_GOTO_EOF:
			cur->x = 0;
			cur->y = buffer->last_line-1;
			break;

		case KEY_GOTO_SOF:
			cur->x = 0;
			cur->y = 0;
			break;
		
		case KEY_GOTO_EOL:
			if(cur->y < buffer->last_line) {
				cur->x = buffer->data[cur->y].len;
			}
			break;

		case KEY_GOTO_SOL:
			cur->x = 0;  // TODO: check for tabs ?
			break;

		case KEY_REPLACE_MODE:
			buffer->mode = MODE_REPLACE;
			break;
		
		case KEY_INSERT_MODE:
			buffer->mode = MODE_INSERT;
			break;

		case KEY_SELECT_MODE:
			buffer->mode = MODE_SELECT;
			break;

		case ENTER:
			handle_enter_key(buffer, cur);
			break;

		case BACKSPACE:
			handle_backspace_key(buffer, cur);
			break;

		case ESC:
			plx_keyinput(); // ignore 0x5b '['
			switch(plx_keyinput()) {	
				case 'C':
					if(cur->x < buffer->data[cur->y].len) {
						cur->x++;
						cur->prev_line_x = cur->x;
					}
					break;

				case 'D':
					if(cur->x > 0) {
						cur->x--;
						cur->prev_line_x = cur->x;
					}
					break;

				case 'A':
					if(cur->y > 0) {
						cur->y--;
						cur->x = MIN(cur->prev_line_x, buffer->data[cur->y].len);
					}
					break;
	
				case 'B':
					if(cur->y < buffer->last_line - 1) {
						cur->y++;
						cur->x = MIN(cur->prev_line_x, buffer->data[cur->y].len);
					}
					break;
			
				default: break;
			}
			break;

		default:
			if(buffer_addchr(buffer, cur->x, cur->y, input)) {
				cur->x++;
				cur->prev_line_x = cur->x;
			}
			break;
	}
}


int main() {
	
	plx_init();
	if(plx_getstatus() & PLX_ERR) {
		plx_exit();
		return -1;
	}

	struct plx_font font;
	struct cursor cur;
	struct edit_buffer buffer;

	plx_load_font(FONT_FILE, &font);
    font.scale = 2;
	font.spacing = 1;

	u32 width = 0;
	u32 height = 0;

	plx_getres(&width, &height);
    plx_clear_color(10, 10, 83);
	plx_swap_buffers();


	char title[64];
	char* modes[4] = {
		"MODE_INVALID",
		"MODE_REPLACE",
		"MODE_INSERT",
		"MODE_SELECT"
	};

	const u32 font_scr_width = (font.header.width + font.spacing) * font.scale;
	const u32 font_scr_height = (font.header.height + font.spacing) * font.scale;
	const u32 title_y = TITLE_POS ? 0 : height - font_scr_height;

	create_buffer(&buffer, 
			width / font_scr_width,
			height / font_scr_height
			);

	cur.x = 0;
	cur.y = 0;
	cur.prev_line_x = 0;

    while(1) {
		
		plx_color(80, 200, 80);
        plx_draw_rect(MIN(cur.x, buffer.width) * font_scr_width, (cur.y + TITLE_POS) * font_scr_height, font_scr_width, font_scr_height);
		
		plx_color(80, 80, 80);
		plx_draw_rect(0, title_y, width, font_scr_height);
		
		sprintf(title, "%s | %li | %ix%i | %i,%i", modes[buffer.mode], buffer.last_line, width, height, cur.x, cur.y);
		plx_color(25, 255, 255);
		plx_draw_text(TITLE_OFFSET, title_y, title, strlen(title), &font);

		// (TODO: dont use time to render stuff that is actually not visible at the moment.)
		plx_color(255, 255, 255);
		for(u64 i = 0; i < buffer.last_line; i++) {
			const struct string* str = &buffer.data[i];
			plx_draw_text(0, (i + TITLE_POS) * font_scr_height, str->data, MIN(buffer.width, str->len), &font);
		}

		plx_swap_buffers();
		handle_input(&buffer, &cur);
    }

    plx_unload_font(&font);
    plx_exit();
}

