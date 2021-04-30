#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>

#include "typedefs.h"

#define MAX(a, b) ((a >= b) ? a : b)
#define MIN(a, b) ((a <= b) ? a : b)


u8 allocate_memory(void** ptr, u64 mem_size);
u8 resize_memory(void** ptr, u64 mem_size, u64 new_mem_size);


#endif
