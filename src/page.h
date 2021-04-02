#ifndef PAGE_H
#define PAGE_H

#include "def.h"


struct EDIPage {
	char* name;
	char* text;

	UINT16 index;
	UINT32 line_count;
	UINT64 size;
	UINT64 max_size;
};


#define EDI_DEFAULT_PAGE_NAME "<Empty>"
#define EDI_DEFAULT_PAGE_SIZE 512

UINT16 EDICreateNewPage(const char* t_name);          // Create new page with name but current page doesnt change.
UINT16 EDICreateNewCurrentPage(const char* t_name);   // Create new page with name but set it as current.
void EDIDeleteAllPages();                             // Free all created pages.
void EDIDeletePage(const UINT16 t_index);             // Free one specific page.
void EDISetCurrentPage(const UINT16 t_index);         // Set page as current

UINT16 EDIGetMaxPage();

struct EDIPage* EDIGetPage(const UINT16 t_index);          // Get page at index.                (Warning: defaults to NULL)
struct EDIPage* EDIGetCurrentPage();                       // Get current page that was set.    (Warning: defaults to NULL)


#endif

