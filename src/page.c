#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "page.h"


static struct EDIPage* g_pages = NULL;
static UINT16 g_current_page = 0;
static UINT16 g_num_pages = 0;

// -----------------------------


UINT16 EDICreateNewPage(const char* t_name) {
	UINT16 return_index = 0;

	if(g_num_pages > 0) {
		g_pages = (struct EDIPage*)realloc(g_pages, (g_num_pages+1)*sizeof(struct EDIPage));
	} else {
		g_pages = (struct EDIPage*)malloc(sizeof(struct EDIPage));
	}

	if(g_pages != NULL) {
		struct EDIPage* page = &g_pages[g_num_pages];
		if(page != NULL) {
			const char* const name_string = (t_name == NULL || !strcmp(t_name, "")) ? EDI_DEFAULT_PAGE_NAME : t_name;
			page->name = (char*)malloc(sizeof(char)*strlen(name_string));
			page->text = (char*)malloc(sizeof(char)*EDI_DEFAULT_PAGE_SIZE);
			
			strcpy(page->name, name_string);
			strcpy(page->text, "");
			
			return_index = g_num_pages;
			page->index = g_num_pages;
			page->size = 0;
			page->line_count = 0;
			page->max_size = EDI_DEFAULT_PAGE_SIZE;
			
			g_num_pages++;
		}
	}
	return return_index;
}

UINT16 EDICreateNewCurrentPage(const char* t_name) {
	return (g_current_page = EDICreateNewPage(t_name));
}

void EDIDeleteAllPages() {
	if(g_num_pages > 0) {
		for(UINT16 i = 0; i < g_num_pages; i++) {
			struct EDIPage* page = &g_pages[i];
			free(page->name);
			free(page->text);
			page->index = 0;
			page->size = 0;
			page->line_count = 0;
		}
		free(g_pages);
		g_pages = NULL;
		g_num_pages = 0;
	}
}

void EDIDeletePage(const UINT16 t_index) {
	if(t_index < g_num_pages) {
		struct EDIPage* page = EDIGetPage(t_index);
		free(page->name);
		free(page->text);
		page->index = 0;
		page->size = 0;
		page->line_count = 0;

		for(UINT16 i = t_index; i < g_num_pages-1; i++) {
			g_pages[i] = g_pages[i+1];
			g_pages[i].index = i;
		}
		
		g_pages = (struct EDIPage*)realloc(g_pages, (g_num_pages-1)*sizeof(struct EDIPage));
		g_num_pages--;
	}
}

void EDISetCurrentPage(const UINT16 t_index) {
	g_current_page = (t_index >= g_num_pages) ? g_num_pages-1 : t_index;
}

UINT16 EDIGetNumPages() {
	return g_num_pages;
}


struct EDIPage* EDIGetPage(const UINT16 t_index) {
	struct EDIPage* page = NULL;
	if(g_num_pages > 0 && t_index < g_num_pages) {
		page = &g_pages[t_index];
		if(page != NULL && page->text != NULL) {
			page->size = strlen(page->text);
			page->text[page->size+1] = '\0';
		}
	}
	return page;
}

struct EDIPage* EDIGetCurrentPage() {
	return EDIGetPage(g_current_page);
}

