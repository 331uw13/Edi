#include <stdio.h>
#include <errno.h>

#include "util.h"


u8 allocate_memory(void** ptr, u64 mem_size) {
	u8 res = 0;
	void* tmpptr = malloc(mem_size);
	if(tmpptr != NULL) {
		*ptr = tmpptr;
		res = 1;
	}
	else {
		fprintf(stderr, "Failed to allocate %i bytes of memory!\n", mem_size);
		if(errno == ENOMEM) {
			fprintf(stderr, "out of memory!\n");
		}
	}

	return res;
}

u8 resize_memory(void** ptr, u64 mem_size, u64 new_mem_size) {
	u8 res = 0;
	if(mem_size < new_mem_size) {
		void* tmpptr = realloc(*ptr, new_mem_size);
		if(tmpptr != NULL) {
			*ptr = tmpptr;
			res = 1;
		}
		else {
			fprintf(stderr, "Failed to allocate %i bytes of memory!\n", (new_mem_size - mem_size));
			if(errno == ENOMEM) {
				fprintf(stderr, "out of memory!\n");
			}
		}
	}
	return res;
}

int lua_getint(lua_State* lua_state, char* name) {
	int res = 0;
	lua_getglobal(lua_state, name);
	if(lua_isnumber(lua_state, -1)) {
		res = lua_tonumber(lua_state, -1);
	}
	lua_pop(lua_state, -1);
	return res;
}

