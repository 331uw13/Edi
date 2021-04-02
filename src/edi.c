#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>

#include "edi.h"
#include "text.h"


#define EDIColorRGB(R, G, B) (R<<16)|(G<<8)|(B<<0)

#define DEFAULT_BORDER_COLOR 0x005500
#define DEFAULT_BG_COLOR 0x000000

#define EDI_QUIT (1 << 0)
// ...



static Window g_window;
static Display* g_display = NULL;       // Connection to the X server.
static int g_screen_num = 0;            // File descriptor of the connection to the X server.
static int g_state = 0;                 // Store global state of the program.

struct cursor__s {
	int x;
	int y;
	int screen_x;
	int screen_y;
	char chr;   // current character on cursor position.
} static cursor;

// -----------------------------


void EDIHandleKeyPress(XKeyEvent* t_event, const struct EDIPage* t_page) {
	if(t_page == NULL) { return; }

	char key = -1;
	KeySym keysym;
	const int length = XLookupString(t_event, &key, 1, &keysym, NULL);

	if(key > 0 && length > 0) {
		//printf("%c / 0x%x\n", key, key);
		
		if(EDIAddChar(EDIGetCurrentPage(), key, cursor.x, 0)) {
			cursor.x++;
		}
			
	} else {
		switch(keysym) {

			case XK_Left:
				if(cursor.x-1 >= 0) {
					cursor.x--;
				}
				break;
			
			case XK_Right:
				cursor.x++;
				break;

			case XK_Up:
				if(cursor.y-1 >= 0) {
					cursor.y--;
				}
				break;
			
			case XK_Down:
				cursor.y++;
				break;


			default: break;
		}
	}
}


void EDIStart(const char* t_filename) {

	g_display = XOpenDisplay(NULL);
	if(g_display == NULL) {
		fprintf(stderr, "Cannot open display.\n");
		return;
	}

	/*
	for(UINT16 i = 0; i < EDIGetNumPages(); i++) {
		struct EDIPage* page = EDIGetPage(i);
		page->line_count = i*3;
		printf("\n\n\033[94m ==== page \033[34m%i\033[94m ------------->\033[0m\n", i);
		printf("  name: \033[36m\"%s\"\033[0m\n", page->name);
		printf(" \033[36m/\n\033[90m%s\033[36m\n \\\033[0m\n", page->text);
		printf("  index: \033[35m%i\033[0m\n", page->index);
		printf("  size: \033[35m%li\033[0m\n", page->size);
		printf("  line_count: \033[35m%i\033[0m\n", page->line_count);
	}
	*/

	/*
	
		https://tronche.com/gui/x/xlib/graphics/font-metrics/querying-string-size.html

	*/


	Atom wmdw = XInternAtom(g_display, "WM_DELETE_WINDOW", 1);

	g_screen_num = XConnectionNumber(g_display);
	g_window = XCreateSimpleWindow(g_display, XDefaultRootWindow(g_display), 0, 0, 500, 350, 2, DEFAULT_BORDER_COLOR, DEFAULT_BG_COLOR);
	XSetWMProtocols(g_display, g_window, &wmdw, 1);
	
	XSelectInput(g_display, g_window, ExposureMask | KeyPressMask); // Receive expose and keypress events
	XMapWindow(g_display, g_window);
	XStoreName(g_display, g_window, "Edi");

	Font font_id = XLoadFont(g_display, "-misc-tamsyn-medium-r-normal--16-116-100-100-c-80-iso8859-1");
	XFontStruct* font = XQueryFont(g_display, font_id);
	GC context = XCreateGC(g_display, g_window, 0, 0);

	XSetWindowBackground(g_display, g_window, EDIColorRGB(24, 24, 23));
	XSetFont(g_display, context, font_id);
	XWindowAttributes win_attr;
	
	const UINT32 char_width = XTextWidth(font, " ", 1);
	const UINT32 char_height = char_width*2;

	EDICreateNewPage(t_filename);
	XEvent event;

	struct EDIPage* page = EDIGetCurrentPage();

	while(!(g_state & EDI_QUIT)) {
		XNextEvent(g_display, &event);
		switch(event.type) {

			case ClientMessage:
				if(event.xclient.data.l[0] == wmdw) {
					g_state |= EDI_QUIT;
				}
				break;

			case Expose:
				XGetWindowAttributes(g_display, g_window, &win_attr);
				//XSetForeground(g_display, context, EDIColorRGB(20, 20, 20));
				//XFillRectangle(g_display, g_window, context, 0, 0, wattr.width, wattr.height);
				break;

			case KeyPress:
				EDIHandleKeyPress(&event.xkey, page);
				cursor.screen_x = (cursor.x+1)*char_width;
				cursor.screen_y = (cursor.y*char_height);
				
				XClearWindow(g_display, g_window);
				//XClearArea(g_display, g_window, cursor.screen_x-char_width, cursor.screen_y-char_height-char_width, char_width*3, win_attr.height*3, 0);

				XSetForeground(g_display, context, EDIColorRGB(30, 180, 30));
				XFillRectangle(g_display, g_window, context, cursor.screen_x, cursor.screen_y+2, char_width, char_height);

				XSetForeground(g_display, context, EDIColorRGB(255, 230, 210));
				XDrawString(g_display, g_window, context, char_width, char_height, EDIGetCurrentPage()->text, EDIGetCurrentPage()->size);
				XFlushGC(g_display, context);
				break;

			default: break;
		}
	}

	//XUnloadFont(g_display, font_id);
	XFreeFont(g_display, font);
	XFreeGC(g_display, context);
	EDIExit();
}

void EDIExit() {
	EDIDeleteAllPages();
	XDestroyWindow(g_display, g_window);
	XCloseDisplay(g_display);
}


