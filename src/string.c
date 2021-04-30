#include <stdio.h>

#include "string.h"
#include "memory_utils.h"


void create_string(struct string* str, u64 bytes) {
	if(str != NULL) {
		if(allocate_memory((void*)&str->data, bytes)) {
			str->len = 0;
			str->alloc_size = bytes;
		}
	}
}

void destroy_string(struct string* str) {
	if(str != NULL && str->data != NULL) {
		free(str->data);
		str->data = NULL;
		str->alloc_size = 0;
		str->len = 0;
	}
	else {
		printf("Warning: possible memory leak!\nstring data is NULL when destroying it!\n");
	}
}

void swap_string(struct string* a, struct string* b) {
	struct string n = *a;
	*a = *b;
	*b = n;
}

void cut_string(struct string* from, struct string* to) {
	memmove(to->data, from->data, from->len);
	to->len = from->len;
}

