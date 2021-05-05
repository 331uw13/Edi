#include <stdlib.h>

#include "input_handler.h"
#include "command_handler.h"
#include "util.h"
#include "config.h"
#include "plx.h"

#define ENTER      0xD
#define BACKSPACE  0x7F
#define TAB        0x9
#define ESC        0x1B


void handle_input(struct edi* e) {
	u8 input = plx_keyinput();

	switch(input) {
		
		case KEY_QUIT_EDITOR:
			exit_program(e);
			break;

		case KEY_SWAP_LINE_UP:
			if(e->buf->cursor.y > 0) {
				swap_string(&e->buf->data[e->buf->cursor.y-1], &e->buf->data[e->buf->cursor.y]);
				e->buf->cursor.y--;
			}
			break;
		
		case KEY_SWAP_LINE_DOWN:
			if(e->buf->cursor.y < e->buf->last_line) {
				swap_string(&e->buf->data[e->buf->cursor.y], &e->buf->data[e->buf->cursor.y+1]);
				e->buf->cursor.y++;
			}
			break;

		case KEY_GOTO_EOF:
			e->buf->cursor.x = 0;
			e->buf->cursor.y = e->buf->last_line-1;
			break;

		case KEY_GOTO_SOF:
			e->buf->cursor.x = 0;
			e->buf->cursor.y = 0;
			break;
		
		case KEY_GOTO_EOL:
			if(e->buf->cursor.y < e->buf->last_line) {
				e->buf->cursor.x = e->buf->data[e->buf->cursor.y].len;
			}
			break;

		case KEY_GOTO_SOL:
			e->buf->cursor.x = 0;  // TODO: check for tabs ?
			break;

		case KEY_COMMAND_INPUT:
			e->buf->mode = COMMAND_INPUT;
			e->buf->cursor.x = 0;
			break;

		case KEY_SWITCH_MODE:
			switch(plx_keyinput()) {
			
				case KEY_INSERT_MODE:
					e->buf->mode = MODE_INSERT;
					break;

				case KEY_REPLACE_MODE:
					e->buf->mode = MODE_REPLACE;
					break;

				case KEY_SELECT_MODE:
					e->buf->mode = MODE_SELECT;
					break;

				default: break;
			}
			break;

		case ENTER:
			handle_enter_key(e->buf);
			break;

		case BACKSPACE:
			handle_backspace_key(e->buf);
			break;

		case ESC:
			handle_cursor_movement(e->buf);
			break;

		default:
			if(input == TAB || input >= 0x20 && input < 0x7F) {
				if(buffer_addchr(e->buf, e->buf->cursor.x, e->buf->cursor.y, input)) {
					buffer_move_cursor(e->buf, 1, 0);
				}
			}
			break;
	}
}


void handle_command_input(struct edi* e) {
	const u8 input = plx_keyinput();

	switch(input) {
	
		case ENTER:
			e->buf->mode = MODE_INSERT;
			e->buf->cursor.x = 0;
			handle_command_string(e, &e->buf->cmd_input);

			memset(e->buf->cmd_input.data, 0x0, e->buf->cmd_input.len);
			e->buf->cmd_input.len = 0;
			break;

		case BACKSPACE:
			if(string_shift(&e->buf->cmd_input, e->buf->cursor.x, -1)) {
				e->buf->cursor.x--;
			}
			break;

		case ESC:
			handle_cursor_movement(e->buf);
			break;
	
		default:
			if(input >= 0x20 && input < 0x7F) {
				if(string_shift(&e->buf->cmd_input, e->buf->cursor.x, 1)) {
					e->buf->cmd_input.data[e->buf->cursor.x] = input;
					buffer_move_cursor(e->buf, 1, 0);
				}
			}
			break;
	}
}

void handle_cursor_movement(struct edit_buffer* buffer) {
	plx_keyinput(); // Used to ignore '['.
	switch(plx_keyinput()) {
		case 'C':  // LEFT
			buffer_move_cursor(buffer, 1, 0);
			break;

		case 'D':  // RIGHT
			buffer_move_cursor(buffer, -1, 0);
			break;

		case 'A':  // UP
			buffer_move_cursor(buffer, 0, -1);
			break;
	
		case 'B':  // DOWN
			buffer_move_cursor(buffer, 0, 1);
			break;
	
		default: break;
	}
}

static void handle_enter_key(struct edit_buffer* buffer) {
	if(buffer_health_check(buffer) && buffer->cursor.y < buffer->last_line) {
		struct string* str = &buffer->data[buffer->cursor.y];
		const u32 x = MIN(buffer->cursor.x, str->len);
		const u32 y = MIN(buffer->cursor.y, buffer->last_line);

		if(buffer_shift_lines(buffer, BUFFER_SHIFT_DOWN, y)) {
			if(buffer_cut(buffer, str, x, str->len, 0, y+1)) {
				buffer->cursor.x = 0;
				buffer->cursor.y++;

				if(str->data[0] == '\t') {
					u32 num_at_start = 1;
					for(u32 i = 1; i < str->len; i++) {
						if(str->data[i] == '\t') {
							num_at_start++;
						}
						else {
							break;
						}
					}
					str = &buffer->data[buffer->cursor.y];
					string_add_char(str, 0, num_at_start, '\t');
					buffer->cursor.x += num_at_start;
				}
			}
		}
	}
}

static void handle_backspace_key(struct edit_buffer* buffer) {
	if(buffer_health_check(buffer) && buffer->cursor.y < buffer->last_line) {
		struct string* str = &buffer->data[buffer->cursor.y];
		const u32 len = str->len;

		if(buffer->cursor.x == 0 && buffer->cursor.y > 0) {
			if(buffer_shift_lines(buffer, BUFFER_SHIFT_UP, buffer->cursor.y)) {
				buffer_move_cursor(buffer, 0, -1);
				buffer->cursor.x = buffer->data[buffer->cursor.y].len - len;
			}
		}
		else {
			if(buffer_remchr(buffer, buffer->cursor.x, buffer->cursor.y)) {
				buffer_move_cursor(buffer, -1, 0);
			}
		}
	}
}


