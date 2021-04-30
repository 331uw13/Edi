#include <stdio.h>

#include "buffer.h"
#include "memory_utils.h"


void create_buffer(struct edit_buffer* buffer, u32 width, u32 height) {
	if(buffer != NULL) {
		// Allocate memory to store lines.
		if(allocate_memory((void*)&buffer->data, sizeof(struct string) * height)) {
			// Now allocate memory for lines to store data.
			for(u32 i = 0; i < height; i++) {
				create_string(&buffer->data[i], width);
			}
			
			buffer->mode = MODE_INSERT;
			buffer->last_line = height-1;
			buffer->width = width;
			buffer->height = height;
		}
	}
}

void destroy_buffer(struct edit_buffer* buffer) {
	if(buffer != NULL && buffer->data != NULL) {
		for(u64 i = 0; i < buffer->last_line; i++) {
			destroy_string(&buffer->data[i]);
		}
		free(buffer->data);
		buffer->data = NULL;
		buffer->mode = MODE_INVALID;
		buffer->last_line = 0;
		buffer->width = 0;
		buffer->height = 0;
	}
	else {
		printf("Warning: possible memory leak!\nbuffer is NULL when destroying it!\n");
	}
}

u8 buffer_health_check(struct edit_buffer* buffer) {
	return (
			buffer != NULL &&
			buffer->data != NULL &&
			buffer->mode != MODE_INVALID &&
			buffer->width > 1 &&
			buffer->height > 1
			);
}

u8 buffer_addchr(struct edit_buffer* buffer, u32 x, u32 y, char c) {
	u8 res = 0;
	if(buffer_health_check(buffer) && y < buffer->last_line) {
		struct string* str = &buffer->data[y];
		
		if(x < str->alloc_size && x < str->len+1 && str->len+1 < str->alloc_size) {
			if(buffer->mode == MODE_INSERT) {
				memmove(str->data+x+1, str->data+x, str->len - x);  // Shift to right.
				str->len++;
			}
			str->data[x] = c;
			res = 1;
		}
	}

	return res;
}

u8 buffer_remchr(struct edit_buffer* buffer, u32 x, u32 y) {
	u8 res = 0;
	if(buffer_health_check(buffer) && y < buffer->last_line) {
		struct string* str = &buffer->data[y];

		if(buffer->mode == MODE_INSERT) {
			if(x < str->alloc_size && x < str->len+1 && x > 0 && str->len > 0) {
				memmove(str->data+x-1, str->data+x, str->len - x);
				str->len--;
				res = 1;
			}
		}
	}

	return res;
}

u8 buffer_shift_lines(struct edit_buffer* buffer, u8 dir, u32 y) {
	u8 res = 0;
	if(buffer_health_check(buffer) && y < buffer->last_line) {

		if(dir == BUFFER_SHIFT_DOWN) {
			for(u32 i = buffer->last_line; i > y; i--) {
				
				cut_string(&buffer->data[i-1], &buffer->data[i]);
				/*
				struct string* dest = &buffer->data[i];
				struct string* src = &buffer->data[i-1];

				memmove(dest->data, src->data, src->len);

				dest->len = src->len;
				dest->alloc_size = src->alloc_size;
				*/
			}
			res = 1;
		}
		else if(dir == BUFFER_SHIFT_UP && y > 0) {
			for(u32 i = y; i < buffer->last_line; i++) {
				cut_string(&buffer->data[i+1], &buffer->data[i]);
			}
			res = 1;
		}
	}

	return res;
}







