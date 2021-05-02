#ifndef EDI_H
#define EDI_H

#include "buffer.h"
#include "frame.h"
#include "config.h"
#include "util.h"


struct edi {
	u32 buffer_count;
	u32 frame_count;
	u32 empty_buffer_id;
	u32 empty_frame_id;
	lua_State* lua_state;
	struct frame         frames[MAX_FRAMES];
	struct edit_buffer   buffers[MAX_BUFFERS];	
	struct edit_buffer*  buf;                  // Pointer to current buffer
	struct frame*        fr;                   // Pointer to active frame. NULL if none is active.
};

void init_program(struct edi* e);
void exit_program(struct edi* e);

u8 buffer_is_empty(struct edit_buffer* buffer);
u32 find_empty_buffer(struct edi* e);
u32 find_empty_frame(struct edi* e);
u8 add_buffer(struct edi* e, u32 cols, u32 rows);
u8 add_frame(struct edi* e, char* text, u32 x, u32 y, u32 w, u32 h);
void remove_buffer(struct edi* e, u32 id);
void remove_frame(struct edi* e, u32 id);


#endif
