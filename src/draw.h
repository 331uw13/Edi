#ifndef DRAW_H
#define DRAW_H

#include "common.h"


void init_drawing();
void free_drawing();

u32 get_max_col();
u32 get_max_row();
u32 get_font_width();
u32 get_font_height();

void clear_screen(u32 bg_color);
void clear_space(u16 col, u16 row, u16 width);
void draw_char(char c, u16 col, u16 row, u32 fg_color, u32 bg_color);
void draw_text(char* text, u32 len, u16 col, u16 row, u32 fg_color, u32 bg_color);
void draw_rect(u16 row, u16 col, u16 w, u16 h, u32 color);

void draw_text_with_width_map(char* text, u32 len, u16 col, u16 row, u32 fg_color, u32 bg_color);


#endif
