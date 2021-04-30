#ifndef EDIT_BUFFER_H
#define EDIT_BUFFER_H

#include "string.h"


#define MODE_INVALID 0
#define MODE_REPLACE 1
#define MODE_INSERT  2
#define MODE_SELECT  3

#define BUFFER_SHIFT_DOWN 0
#define BUFFER_SHIFT_UP 1

struct edit_buffer {
	u32 mode;
	u32 width;
	u32 height;
	u32 last_line;
	struct string* data;
};


void create_buffer(struct edit_buffer* buffer, u32 width, u32 height);
void destroy_buffer(struct edit_buffer* buffer);
u8 buffer_health_check(struct edit_buffer* buffer);

u8 buffer_addchr(struct edit_buffer* buffer, u32 x, u32 y, char c);
u8 buffer_remchr(struct edit_buffer* buffer, u32 x, u32 y);
u8 buffer_shift_lines(struct edit_buffer* buffer, u8 dir, u32 y);


#endif
