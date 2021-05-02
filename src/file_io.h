#ifndef FILE_IO_H
#define FILE_IO_H

#include "buffer.h"


u8 file_exists(char* filename);
void open_file(char* filename, struct edit_buffer* buffer);
void close_file(struct edit_buffer* buffer);
void read_file(struct edit_buffer* buffer);
void write_file(char* filename, struct edit_buffer* buffer);


#endif
