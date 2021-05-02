#ifndef FRAME_H
#define FRAME_H

#include "string.h"

#define FRAME_ACTIVE (1<<0)
#define FRAME_SHOULD_CLOSE (1<<1)


struct frame {
	struct string title;
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	u32 flags;
};


#endif
