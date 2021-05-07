#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>

#include "edi.h"
#include "util.h"
#include "draw.h"


void init_program(struct edi* e) {
	assert(e != NULL);

	e->buffer_count = 0;
	e->frame_count = 0;
	e->empty_buffer_id = 0;
	e->empty_frame_id = 0;
	e->lua_state = NULL;
	e->buf = NULL;
	e->width = 0;
	e->height = 0;
		
	for(u32 i = 0; i < MAX_BUFFERS; i++) {
		struct edit_buffer* buf = &e->buffers[i];
		buf->data = NULL;
		buf->cmd_input.data = NULL;
		buf->cmd_input.len = 0;
		buf->mode = MODE_INVALID;
		buf->has_fd = 0;
		buf->last_line = 0;
	}
	
	for(u32 i = 0; i < MAX_FRAMES; i++) {
		struct frame* fr = &e->frames[i];
		fr->x = 0;
		fr->y = 0;
		fr->width = 0;
		fr->height = 0;
		fr->title.data = NULL;
		fr->title.len = 0;
		fr->flags = 0;
	}

}

void exit_program(struct edi* e) {
	int res = -1;

	/* DELETE THIS */
   	char* cmd = "ps -p $(pidof edi) -o %cpu,%mem > log.txt";
	system(cmd);


	if(e != NULL) {
		free_drawing();
		lua_close(e->lua_state);

		for(u32 i = 0; i < e->buffer_count; i++) {
			free_buffer(&e->buffers[i]);
		}

		for(u32 j = 0; j < e->frame_count; j++)  {
			free_string(&e->frames[j].title);
		}
		
		res = 0;
	}

	exit(res);
}

u8 buffer_is_empty(struct edit_buffer* buffer) {
	u8 res = (
				buffer != NULL &&
				buffer->data == NULL &&
				buffer->mode == MODE_INVALID &&
				buffer->has_fd == 0 &&
				buffer->last_line == 0
			);

	return res;
}


u32 find_empty_buffer(struct edi* e) {
	assert(e != NULL);
	u32 next = MIN(e->empty_buffer_id, MAX_BUFFERS-1);
	if(!buffer_is_empty(&e->buffers[next])) {
		for(u32 i = 0; i < MAX_BUFFERS; i++) {
			if(buffer_is_empty(&e->buffers[i])) {
				next = i;
				break;
			}
		}
	}

	return next;
}

u32 find_empty_frame(struct edi* e) {
	assert(e != NULL);
	u32 next = MIN(e->empty_frame_id, MAX_FRAMES-1);
	if(e->frames[next].title.data != NULL && e->frames[next].title.len != 0) {
		for(u32 i = 0; i < MAX_FRAMES; i++) {
			if(e->frames[i].title.data == NULL && e->frames[i].title.len == 0) {
				next = i;
				break;
			}
		}
	}

	return next;

}

u8 add_buffer(struct edi* e, u32 cols, u32 rows) {
	u8 res = 0;
	if(e != NULL && cols * rows > 1) {
		struct edit_buffer* buf = &e->buffers[find_empty_buffer(e)];
		if(init_buffer(buf, cols, rows)) {
			e->buf = buf;
			e->buffer_count++;
		}
	}

	return res;
}

u8 add_frame(struct edi* e, char* text, u32 x, u32 y, u32 w, u32 h) {
	u8 res = 0;
	if(e != NULL) {
		struct frame* fr = &e->frames[find_empty_frame(e)];
		if(fr->title.data == NULL && fr->title.len == 0) {
			init_string(&fr->title, MAX_FRAME_TITLE_LEN);
			string_add_data(&fr->title, text, strlen(text), 0);
			fr->x = x;
			fr->y = y;
			fr->width = w;
			fr->height = h;
			fr->flags = FRAME_ACTIVE;

			e->fr = fr;
			e->frame_count++;
		}
	}

	return res;
}

void remove_buffer(struct edi* e, u32 id) {
}

void remove_frame(struct edi* e, u32 id) {
}




