#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "file_io.h"

//
// TODO: report errors!
// ---------------------


u8 file_exists(char* filename) {
	return (access(filename, R_OK | W_OK) >= 0);
}

void open_file(char* filename, struct edit_buffer* buffer) {
	if(filename != NULL && buffer != NULL) {
		buffer->fd = open(filename, O_RDWR | O_CREAT);
		if(buffer->fd >= 0) {
			if((buffer->has_fd = (fstat(buffer->fd, &buffer->sb) >= 0))) {
				read_file(buffer);
			}
		}
	}
}

void close_file(struct edit_buffer* buffer) {
	if(buffer != NULL && buffer->has_fd) {
		close(buffer->fd);
		buffer->fd = 0;
		buffer->has_fd = 0;
	}
}

void write_file(char* filename, struct edit_buffer* buffer) {
	u32 last_not_empty = 0;
	for(u32 j = buffer->last_line; j > 0; j--) {
		if(buffer->data[j].len > 0) {
			last_not_empty = j+1;
			break;
		}
	}

	/*
	for(u32 i = 0; i < last_not_empty; i++) {
		write(STDOUT_FILENO, buffer->data[i].data, buffer->data[i].len);
	}
	*/
}

void read_file(struct edit_buffer* buffer) {
	if(buffer->has_fd) {
		if(fstat(buffer->fd, &buffer->sb) >= 0) {
			char* data = mmap(NULL, buffer->sb.st_size, PROT_READ, MAP_PRIVATE, buffer->fd, 0);
			if(data == MAP_FAILED || data == NULL) {
				return;
			}

			u32 p = 0;
			u32 pp = 0;
			u64 size = 0;
			u32 line_len = 0;
			u32 line = 0;
			char* i = NULL;
			//char testbuf[1024] = { 0 };

			do {
				size = buffer->sb.st_size - p;
				i = memchr(data + p, '\n', size);
				line_len = p - pp;
				if(line_len > 0) {
					line_len--; // Remove new line because it is added when writing to file.
				}

				struct string* str = &buffer->data[line];
				if(!string_memcheck(str, line_len)) {
					break;
				}

				memmove(str->data, data + pp, line_len);
				str->len = line_len;
				
				//memmove(testbuf, data + pp, buflen);
				//testbuf[buflen] = 0;
				//printf("> \033[32m%s\033[0m\n", testbuf);

				pp = p;
				p = i - data+1;
				line++;
			} while(i != NULL && p <= buffer->sb.st_size);

			munmap(data, buffer->sb.st_size);
		}
	}
}

