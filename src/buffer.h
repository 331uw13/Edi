#ifndef EDIT_BUFFER_H
#define EDIT_BUFFER_H

#include "string.h"

#define MODE_INVALID 0
#define MODE_REPLACE 1
#define MODE_INSERT  2
#define MODE_SELECT  3

#define BUFFER_SHIFT_DOWN 0
#define BUFFER_SHIFT_UP 1

#define BUFFER_CLEAR_SRC 0
#define BUFFER_IGNORE_SRC 1

struct edit_buffer {
	u32 mode;
	u32 width;
	u32 height;
	u32 last_line;
	u32 last_edited_line;    // This is updated when switching between different buffers.

	u32 scroll_x;
	u32 scroll_y;
	u32 scroll_max_x;
	u32 scroll_max_y;
	
	struct string* data;
	char* title;          /* TODO */
};


void create_buffer(struct edit_buffer* buffer, u32 width, u32 height);
void destroy_buffer(struct edit_buffer* buffer);
u8 buffer_health_check(struct edit_buffer* buffer);

u8 buffer_addchr(struct edit_buffer* buffer, u32 x, u32 y, char c);
u8 buffer_remchr(struct edit_buffer* buffer, u32 x, u32 y);
u8 buffer_cut(struct edit_buffer* buffer, struct string* str, u32 start, u32 end, u32 dest_x, u32 dest_y);
u8 buffer_move(struct edit_buffer* buffer, u32 dest_y, u32 src_y, u8 flag);
u8 buffer_shift_lines(struct edit_buffer* buffer, u8 dir, u32 y);


#endif
