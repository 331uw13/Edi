#include "string.h"
#include "util.h"
#include "config.h"


void init_string(struct string* str, u64 bytes) {
	if(str != NULL) {
		if(allocate_memory((void*)&str->data, bytes)) {
			str->len = 0;
			str->mem_len = bytes;
		}
	}
}

void free_string(struct string* str) {
	if(str != NULL && str->data != NULL) {
		free(str->data);
		str->data = NULL;
		str->mem_len = 0;
		str->len = 0;
	}
	else {
		printf("[CRITICAL WARNING]: possible memory leak!!! String is NULL when trying to destroy it!\n");
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

u8 string_memcheck(struct string* str, int inc) {
	u8 res = 0;
	if(str != NULL) {
		res = 1;
		const int request = str->len + inc;
		if(request >= str->mem_len) {
			if(resize_memory((void*)&str->data, str->mem_len, str->mem_len + LINE_BLOCK_SIZE)) {
				str->mem_len += LINE_BLOCK_SIZE;
			}
			else {
				res = 0;
			}
		}
		else if(request < 0) {
			// TODO: check if its good idea to free some memory.
			res = 0;
		}
	}

	return res;
}

u32 string_num_chars(struct string* str, u32 start, u32 end, char c) {
	u32 count = 0;
	if(str != NULL && str->data != NULL) {
		char* i = &str->data[start];
		u32 p = 0;
		while(p < end - start) {
			i = memchr(str->data + p, c, end - p);
			p = i - str->data + 1;
			count++;
		}
	}

	return count - 1;
}

u8 string_shift(struct string* str, int start, int amount) {
	u8 res = 0;
	if(string_memcheck(str, amount) && (start > 0 || amount > 0)) {
		memmove(str->data + start + amount, str->data + start, str->len - start);
		str->len += amount;
		res = 1;
	}

	return res;
}

void string_add_char(struct string* str, u32 start, u32 num, char c) {
	if(string_shift(str, start, num)) {
		memset(str->data + start, c, num);
	}
}

void string_add_data(struct string* str, char* data, u32 len, u32 start) {
	if(string_memcheck(str, len) && start < str->len + len) {
		memmove(str->data + start, data, len);
		str->len += len;
	}
}

