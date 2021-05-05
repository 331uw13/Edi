#ifndef EDIT_BUFFER_STRING_H
#define EDIT_BUFFER_STRING_H

#include "common.h"

struct string {
	char* data;
	u64 len;
	u64 mem_len;
	u32 redraw_len;
};

void init_string(struct string* str, u64 bytes);
void free_string(struct string* str);
void swap_string(struct string* a, struct string* b);
void copy_string(struct string* dest, struct string* src);
void append_string(struct string* dest, struct string* src);
u8 string_memcheck(struct string* str, int inc);            // Used to handle if line needs more memory.
u32 string_num_chars(struct string* str, u32 start, u32 end, char c);   // Count occurance of specific character.
u8 string_shift(struct string* str, int start, int amount);
void string_add_char(struct string* str, u32 start, u32 num, char c);
void string_add_data(struct string* str, char* data, u32 len, u32 start);
u8 string_cmp_data(char* a, u32 a_len, char* b, u32 b_len);

#endif
