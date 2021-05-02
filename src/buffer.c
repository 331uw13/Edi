#include <stdio.h>

#include "file_io.h"
#include "util.h"


u8 init_buffer(struct edit_buffer* buffer, u32 cols, u32 rows) {
	u8 res = 0;
	if(buffer != NULL && cols * rows > 1) {

		if(allocate_memory((void*)&buffer->data, sizeof *buffer->data * rows)) { 
			for(u32 i = 0; i < rows; i++) {
				init_string(&buffer->data[i], cols);
			}

			buffer->mode = MODE_INSERT;
			buffer->fd = 0;
			buffer->has_fd = 0;
			buffer->last_line = rows-1;
			buffer->width = cols;
			buffer->height = rows;
			buffer->cursor_x = 0;
			buffer->cursor_y = 0;
			buffer->cursor_p_ln_x = 0;
			init_string(&buffer->cmd_input, cols);

			res = 1;
		}
	}
	else {
		fprintf(stderr, "Failed to create buffer (%ix%i)\n", cols, rows);
	}

	return res;
}

void free_buffer(struct edit_buffer* buffer) {
	if(buffer != NULL && buffer->data != NULL) {
		for(u64 i = 0; i < buffer->last_line; i++) {
			free_string(&buffer->data[i]);
		}
		free_string(&buffer->cmd_input);
		free(buffer->data);
		close_file(buffer);
		buffer->data = NULL;
		buffer->mode = MODE_INVALID;
		buffer->last_line = 0;
		buffer->width = 0;
		buffer->height = 0;
		buffer->has_fd = 0;
	}
	else {
		printf("[CRITICAL WARNING]: possible memory leak!!! Buffer is NULL when trying to destroy it!\n");
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
	if(buffer_health_check(buffer)) {
		struct string* str = &buffer->data[y];
		if(str != NULL) {
			x = MIN(x, str->len);
			if(buffer->mode == MODE_INSERT) {
				if(string_shift(str, x, 1)) {
					str->data[x] = c;
					res = 1;
				}
			}
			else {
				str->data[x] = c;
				res = 1;
			}
		}
	}

	return res;
}

u8 buffer_remchr(struct edit_buffer* buffer, u32 x, u32 y) {
	u8 res = 0;
	if(buffer_health_check(buffer)) {
		if(x > 0) {
			y = MIN(y, buffer->last_line);
			struct string* str = &buffer->data[y];
			if(str != NULL) {
				if(buffer->mode == MODE_INSERT) {
					res = string_shift(str, x, -1);
				}
			}
		}
	}

	return res;
}

u8 buffer_cut(struct edit_buffer* buffer, struct string* str, u32 start, u32 end, u32 dest_x, u32 dest_y) {
	u8 res = 0;
	if(buffer_health_check(buffer) && str != NULL) {		
		if(string_memcheck(str, 0)) {
			start = MIN(start, str->len);
			end   = MIN(end, str->len);	
			struct string* target = &buffer->data[dest_y];
			const u32 size = end - start;
			if(string_memcheck(target, size)) {
				if(start != end && size > 0) {
					memmove(target->data + dest_x, str->data + start, size);
					target->len += size;
					
					memset(str->data + start, 0x0, size);
					str->len -= size;
				}
				res = 1;
			}
		}
	}

	return res;
}

u8 buffer_move(struct edit_buffer* buffer, u32 dest_y, u32 src_y, u8 flag) {
	u8 res = 0;
	if(buffer_health_check(buffer)) {
		src_y  = MIN(src_y, buffer->last_line);
		dest_y = MIN(dest_y, buffer->last_line);
		struct string* dest = &buffer->data[dest_y];
		struct string* src  = &buffer->data[src_y];

		if(dest->len > 0) {
			append_string(dest, src);
		}
		else {
			copy_string(dest, src);
		}
		if(flag == BUFFER_CLEAR_SRC) {
			memset(src->data, 0x0, src->len);
			src->len = 0;
		}
		res = 1;
	}

	return res;
}

u8 buffer_shift_lines(struct edit_buffer* buffer, u8 dir, u32 y) {
	u8 res = 0;
	if(buffer_health_check(buffer)) {
		y = MIN(y, buffer->last_line);
		if(dir == BUFFER_SHIFT_DOWN) {
			for(u32 i = buffer->last_line; i > y; i--) {
				res = buffer_move(buffer, i+1, i, BUFFER_CLEAR_SRC);
			}
			res = 1;
		}
		else if(dir == BUFFER_SHIFT_UP && y > 0) {
			for(u32 i = y-1; i < buffer->last_line; i++) {
				buffer_move(buffer, i, i+1, BUFFER_CLEAR_SRC);
			}
			res = 1;
		}
	}

	return res;
}


