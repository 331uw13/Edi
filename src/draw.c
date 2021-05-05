#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "plx.h"
#include "config.h"


static struct plx_fb fb;
static struct plx_font font;
static u32* width_map;


void init_drawing() {
	if(!plx_open("/dev/fb0", &fb)) {
		fprintf(stderr, "plx_open() failed!\n");
		return;
	}

	plx_load_font(FONT_FILE, &font);
	font.spacing = FONT_SPACING;
	font.tab_width = TAB_WIDTH;

	width_map = malloc(sizeof *width_map * (get_max_row()+1));
	if(width_map == NULL) {
		fprintf(stderr, "Failed to allocate memory for width_map!\n");
		return;
	}
	memset(width_map, 0, sizeof *width_map * get_max_row());

	plx_clear(&fb);
}

void free_drawing() {
	plx_unload_font(&font);
	plx_close(&fb);
	free(width_map);
}

u32 get_font_width() {
	return font.header.width + font.spacing;
}

u32 get_font_height() {
	return font.header.height;
}

u32 get_max_col() {
	return fb.width / get_font_width();
}

u32 get_max_row() {
	return fb.height / get_font_height();
}

void clear_screen(u32 bg_color) {
	fb.clear_color = bg_color;
	plx_clear(&fb);
}

void clear_space(u16 col, u16 row, u16 width) {
	const u32 w = font.header.width + font.spacing;
	plx_clear_region(&fb, col * w, row * font.header.height, width * w, font.header.height);
}

void draw_char(char c, u16 col, u16 row, u32 fg_color, u32 bg_color) {
	if(c > 0x1F && c < 0x7F) {
		u32 x = col * (font.header.width + font.spacing);
		u32 y = row * font.header.height;
		if(y <= fb.height - font.header.height && x <= fb.width - (font.header.width + font.spacing)) {
			const u32 origin_x = x;
			for(u8 i = 0; i < font.header.height; i++) {
				u8 g = font.data[c * font.header.height + i];
				for(u8 j = 0; j < 8; j++) {
					fb.data[y * fb.width + x] = (g & 0x80) ? fg_color : bg_color;
					g = g << 1;
					x++;
				}
				y++;
				x = origin_x;
			}
		}
	}
}

void draw_text(char* text, u32 len, u16 col, u16 row, u32 fg_color, u32 bg_color) {
	for(u32 i = 0; i < len; i++) {
		char c = text[i];
		if(c != '\t') {
			draw_char(text[i], col, row, fg_color, bg_color);
			col++;
		}
		else {
			col += font.tab_width;
		}
	}
}

void draw_text_with_width_map(char* text, u32 len, u16 col, u16 row, u32 fg_color, u32 bg_color) {
	const u32 total = col + len;

	if(total < width_map[row]) {
		draw_rect(total, row, width_map[row] - total, 1, bg_color);
		width_map[row] = total;
	} else {
		width_map[row] = total;
	}
	
	draw_text(text, len, col, row, fg_color, bg_color);
}

void draw_rect(u16 col, u16 row, u16 w, u16 h, u32 color) {
	const u32 fw = font.header.width + font.spacing;
	fb.draw_color = color;
	plx_draw_region(&fb, col * fw, row * font.header.height, w * fw, h * font.header.height);
}



