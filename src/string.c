#include <stdio.h>

#include "string.h"
#include "util.h"
#include "config.h"


void create_string(struct string* str, u64 bytes) {
	if(str != NULL) {
		if(allocate_memory((void*)&str->data, bytes)) {
			str->len = 0;
			str->mem_len = bytes;
		}
	}
}

void destroy_string(struct string* str) {
	if(str != NULL && str->data != NULL) {
		free(str->data);
		str->data = NULL;
		str->mem_len = 0;
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

// TODO: check dest allocated size
void copy_string(struct string* dest, struct string* src) {
	if(string_memcheck(dest, 0) && string_memcheck(src, 0)) {
		memmove(dest->data, src->data, MIN(src->len, dest->mem_len-1));
		dest->len = src->len;
	}
}

// TODO: check dest allocated size
void append_string(struct string* dest, struct string* src) {
	if(string_memcheck(dest, src->len) && string_memcheck(src, 0)) {
		memmove(dest->data + dest->len, src->data, src->len);
		dest->len += src->len;
	}
}

u8 string_memcheck(struct string* str, u32 inc) {
	u8 res = 0;
	if(str != NULL) {
		res = 1;
		if(str->len + inc >= str->mem_len) {
			if(resize_memory((void*)&str->data, str->mem_len, str->mem_len + LINE_BLOCK_SIZE)) {
				str->mem_len += LINE_BLOCK_SIZE;
			}
			else {
				res = 0;
			}
		}
	}

	return res;
}

