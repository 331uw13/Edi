#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

#include "common.h"

#define MAX(a, b) ((a >= b) ? a : b)
#define MIN(a, b) ((a <= b) ? a : b)
#define RGB(r, g, b) ((r<<16) | (g<<8) | (b<<0))


u8 allocate_memory(void** ptr, u64 mem_size);
u8 resize_memory(void** ptr, u64 mem_size, u64 new_mem_size);
int lua_getint(lua_State* lua_state, char* name);

#endif
