#ifndef EDIT_BUFFER_H
#define EDIT_BUFFER_H

#include <unistd.h>
#include <sys/stat.h>

#include "string.h"


// Modes:
#define MODE_INVALID   0
#define MODE_INSERT    1
#define MODE_REPLACE   2
#define MODE_SELECT    3
#define COMMAND_INPUT  4

#define BUFFER_SHIFT_DOWN  0
#define BUFFER_SHIFT_UP    1
#define BUFFER_CLEAR_SRC   0
#define BUFFER_IGNORE_SRC  1

#define BUFFER_REDRAW_CURSOR      (1<<0)
#define BUFFER_REDRAW_LINE        (1<<1)
#define BUFFER_REDRAW_TEXT        (1<<2)


struct edit_buffer {
	u32 mode;
	u32 flags;
	u32 width;
	u32 height;
	u32 last_line;
	struct v2 cursor;
	struct v2 prev_cursor;  // Previous cursor position. X axis is updated if line len > 0.
	struct v2 scroll;

	/* TODO
	u32 scroll_x;
	u32 scroll_y;
	u32 scroll_max_x;
	u32 scroll_max_y;
	*/
	int fd;
	u8 has_fd;
	struct stat sb;
	struct string* data;         // NOTE: rename to 'lines'
	struct string cmd_input;
};


u8 init_buffer(struct edit_buffer* buffer, u32 cols, u32 rows);
void free_buffer(struct edit_buffer* buffer);
u8 buffer_health_check(struct edit_buffer* buffer);

u8 buffer_addchr(struct edit_buffer* buffer, u32 x, u32 y, char c);
u8 buffer_remchr(struct edit_buffer* buffer, u32 x, u32 y);
u8 buffer_cut(struct edit_buffer* buffer, struct string* str, u32 start, u32 end, u32 dest_x, u32 dest_y);
u8 buffer_move(struct edit_buffer* buffer, u32 dest_y, u32 src_y, u8 flag);
u8 buffer_shift_lines(struct edit_buffer* buffer, u8 dir, u32 y);

void buffer_move_cursor(struct edit_buffer* buffer, int xoff, int yoff);
void buffer_set_cursor(struct edit_buffer* buffer, u32 x, u32 y);

#endif
