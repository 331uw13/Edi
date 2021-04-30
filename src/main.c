#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "plx.h"
#include "buffer.h"
#include "config.h"

#define ENTER      0xD
#define BACKSPACE  0x7F
#define TAB        0x9
#define ESC        0x1B

#define MAX(a, b) (a > b) ? a : b
#define MIN(a, b) (a < b) ? a : b


struct cursor {
	u32 x;
	u32 y;
};

void handle_enter_key(struct edit_buffer* buffer, struct cursor* cur) {
	if(buffer_health_check(buffer) && cur->y < buffer->last_line) {
		struct string* str = &buffer->data[cur->y];
		struct string* next_str = &buffer->data[cur->y+1];
		const u32 x = MIN(cur->x, str->len);
		const u32 y = MIN(cur->y, buffer->last_line);
		const u32 part_len = str->len - x;
		
		if(buffer_shift_lines(buffer, BUFFER_SHIFT_DOWN, y)) {
			memset(next_str->data, 0x0, next_str->len);
			if(str->len > 0 && x < str->len) {
				memmove(next_str->data, str->data + x, part_len);   // Move part to the new line.	
				memset(str->data + x, 0x0, part_len);           // Clean part from original str that is not duplicate.
			}

			str->len -= part_len;
			next_str->len = part_len;
			cur->y++;
			cur->x = 0;
		}
	}
}

void handle_backspace_key(struct edit_buffer* buffer, struct cursor* cur) {
	if(buffer_health_check(buffer) && cur->y < buffer->last_line) {
		struct string* str = &buffer->data[cur->y];
		
		if(cur->x == 0 && cur->y > 0 && str->len == 0) {
			const u32 x = MIN(cur->x, str->len);
			const u32 y = MIN(cur->y, buffer->last_line);

			if(buffer_shift_lines(buffer, BUFFER_SHIFT_UP, y)) {
				cur->y--;
			}
		}
		else {
			if(buffer_remchr(buffer, cur->x, cur->y)) {
				cur->x--;
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
					cur->x++;
					break;

				case 'D':
					if(cur->x > 0) {
						cur->x--;
					}
					break;

				case 'A':
					if(cur->y > 0) {
						cur->y--;
					}
					break;
	
				case 'B':
					cur->y++;
					break;
			
				default: break;
			}
			break;

		default:
			if(buffer_addchr(buffer, cur->x, cur->y, input)) {
				cur->x++;
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
	struct cursor cur = { 0, 0 };
	struct edit_buffer buffer;

	plx_load_font(FONTFILE, &font);
    font.scale = 1;

	u32 width = 0;
	u32 height = 0;

	plx_getres(&width, &height);
    plx_clear_color(10, 10, 83);
	plx_swap_buffers();


	create_buffer(&buffer, 
			width / font.header.width,
			height / font.header.height
			);

	char title[64];
	char* modes[4] = {
		"MODE_INVALID",
		"MODE_REPLACE",
		"MODE_INSERT",
		"MODE_SELECT"
	};

	const u32 title_y = TITLE_POS ? 0 : height - font.header.height;

    while(1) {
	
		plx_color(80, 200, 80);
        plx_draw_rect(cur.x * font.header.width,
				      (cur.y + TITLE_POS) * font.header.height,
					  font.header.width, font.header.height);

		sprintf(title, "%s | %li | %ix%i | %i,%i",
				modes[buffer.mode], buffer.last_line, width, height, cur.x, cur.y);

		plx_color(80, 80, 80);
		plx_draw_rect(0, title_y, width, font.header.height);
		
		plx_color(25, 255, 255);
		plx_draw_text(TITLE_OFFSET, title_y, title, strlen(title), &font);


		// (TODO: dont use time to render stuff that is actually not visible at the moment.)
		plx_color(255, 255, 255);
		for(u64 i = 0; i < buffer.last_line; i++) {
			const struct string* str = &buffer.data[i];
			plx_draw_text(0, (i + TITLE_POS) * font.header.height, str->data, str->len, &font);
		}

		plx_swap_buffers();
		handle_input(&buffer, &cur);
    }

    plx_unload_font(&font);
    plx_exit();
}

