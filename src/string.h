#ifndef EDIT_BUFFER_STRING_H
#define EDIT_BUFFER_STRING_H

#include "typedefs.h"

struct string {
	char* data;
	u64 len;
	u64 mem_len;
};

void create_string(struct string* str, u64 bytes);
void destroy_string(struct string* str);
void swap_string(struct string* a, struct string* b);
void copy_string(struct string* dest, struct string* src);
void append_string(struct string* dest, struct string* src);
u8 string_memcheck(struct string* str, u32 inc);     // Used to handle if line needs more memory.


#endif
