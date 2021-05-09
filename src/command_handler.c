#include "command_handler.h"
#include "draw.h"


void handle_command_string(struct edi* e, struct string* str) {
	if(e != NULL && e->buf != NULL && str != NULL) {
		
		e->buf->cmd_input.data[MIN(e->buf->cmd_input.len, e->buf->cmd_input.mem_len)] = '\0';
		luaL_dostring(e->lua_state, e->buf->cmd_input.data);
	}
}

