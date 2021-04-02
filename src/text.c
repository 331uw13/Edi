#include <stdlib.h>
#include "text.h"



UINT8 EDIAddChar(struct EDIPage* t_page, const char t_chr, const int t_x, const int t_y) {
	UINT8 result = 0;
	if(t_page != NULL && t_page->text != NULL) {
		if(t_x < t_page->max_size) {
			if(t_page->size > 0) {
				for(UINT32 i = t_page->size+1; i > t_x; i--) {
					t_page->text[i] = t_page->text[i-1];
				}
			}

			t_page->text[t_x] = t_chr;
			
			// TODO: resizing.
			
			result = 1;
		}
	}
	return result;
}


