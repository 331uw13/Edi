#include "command_handler.h"


void handle_command_string(struct edi* e, struct string* str) {
	if(e != NULL && str != NULL && str->len > 0) {
		str->data[str->len] = '\0';

		const int result = luaL_dostring(e->lua_state, str->data);
		if(result != 0) {
			add_msg(e, "%s", lua_tostring(e->lua_state, -1));
		}
	}
}


