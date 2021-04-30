#ifndef EDIT_BUFFER_STRING_H
#define EDIT_BUFFER_STRING_H

#include "typedefs.h"

struct string {
	char* data;
	u64 len;
	u64 alloc_size;
};

void create_string(struct string* str, u64 bytes);
void destroy_string(struct string* str);
void swap_string(struct string* a, struct string* b);
void cut_string(struct string* from, struct string* to);


#endif
