#ifndef EDI_H
#define EDI_H

#include <X11/Xlib.h>

#include "page.h"


void EDIStart(const char* t_filename);
void EDIExit();

void EDIHandleKeyPress(XKeyEvent* t_event, const struct EDIPage* t_page);


#endif
