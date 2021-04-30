#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include <stdlib.h>
#include <string.h>

#include "typedefs.h"


u8 allocate_memory(void** ptr, u64 mem_size);
u8 resize_memory(void** ptr, u64 mem_size, u64 new_mem_size);

#endif
